import { useEffect, useRef, useState } from "react";
import type { User } from "firebase/auth";
import { doc, onSnapshot } from "firebase/firestore";
import type { Curriculum, Evaluation, Offering, Profile } from "../services/api";
import { academicApi } from "../services/academic";
import { planStorage } from "../services/planStorage";
import { db } from "../services/firebase";
import { message } from "../utils/errors";
import { hasScheduleConflict } from "../utils/schedule";
import { usePlannerRoute } from "./usePlannerRoute";

export function usePlanner(user: User) {
  const { route, go: navigate } = usePlannerRoute();
  const [profile, setProfile] = useState<Profile | null | undefined>(undefined);
  const [catalog, setCatalog] = useState<Curriculum>();
  const [completed, setCompleted] = useState<string[]>();
  const [evaluation, setEvaluation] = useState<Evaluation>();
  const [plan, setPlan] = useState<string[]>([]);
  const [savedPlan, setSavedPlan] = useState<string[]>([]);
  const [error, setError] = useState("");
  const [notice, setNotice] = useState("");
  const [busy, setBusy] = useState(false);
  const [retry, setRetry] = useState(0);
  const [search, setSearch] = useState("");
  const [confirmRemoval, setConfirmRemoval] = useState(false);
  const [chooser, setChooser] = useState(false);
  const historyVersion = useRef(0);
  const dirty = JSON.stringify(plan) !== JSON.stringify(savedPlan);

  useEffect(() => {
    if (!dirty) {
      return;
    }

    const warn = (event: BeforeUnloadEvent) => {
      event.preventDefault();
    };

    window.addEventListener("beforeunload", warn);
    return () => window.removeEventListener("beforeunload", warn);
  }, [dirty]);

  useEffect(() => {
    let active = true;
    let unsubscribe: (() => void) | undefined;
    let refreshVersion = 0;

    async function refreshAcademicData() {
      const version = ++refreshVersion;
      const history = historyVersion.current;
      try {
        const [curriculum, summary] = await Promise.all([academicApi.curriculum(), academicApi.summary()]);
        if (!active || version !== refreshVersion) return;
        if (history !== historyVersion.current) {
          void refreshAcademicData();
          return;
        }
        setCatalog(curriculum);
        setCompleted(summary.completed);
        setEvaluation(summary);
      } catch (cause) {
        if (active && version === refreshVersion) setError(message(cause));
      }
    }

    academicApi
      .profile()
      .then(async ({ profile: value }) => {
        if (!active) {
          return;
        }
        setProfile(value);
        if (!value) {
          return;
        }
        const [curriculum, summary, saved] = await Promise.all([
          academicApi.curriculum(),
          academicApi.summary(),
          planStorage.load(user.uid),
        ]);

        if (!active) {
          return;
        }

        setCatalog(curriculum);
        setCompleted(summary.completed);
        setEvaluation(summary);
        const ids = saved;
        setPlan(ids);
        setSavedPlan(ids);
        unsubscribe = onSnapshot(doc(db, "courses", curriculum.id), { includeMetadataChanges: true }, (snapshot) => {
          if (active && !snapshot.metadata.fromCache) void refreshAcademicData();
        }, (cause) => {
          if (active) setError(message(cause));
        });
      })
      .catch((e) => {
        if (active) {
          setError(message(e));
        }
      });
    return () => {
      active = false;
      unsubscribe?.();
    };
  }, [user.uid, retry]);

  function go(next: string) {
    navigate(next);
    setConfirmRemoval(false);
    setNotice("");
  }

  async function perform(action: () => Promise<void>) {
    setBusy(true);
    setError("");
    setNotice("");
    try {
      await action();
    } catch (e) {
      setError(message(e));
    } finally {
      setBusy(false);
    }
  }
  const subject = catalog?.subjects.find(
    (item) => item.id === route.split("/")[1],
  );

  const state = subject && evaluation?.subjects[subject.id];

  const selected =
    catalog?.offerings.filter((item) => plan.includes(item.id)) || [];

  const missingOfferings = plan.filter(
    (id) => !catalog?.offerings.some((item) => item.id === id),
  );

  const conflicts: [Offering, Offering][] = [];

  selected.forEach((a, index) =>
    selected.slice(index + 1).forEach((b) => {
      if (
        a.times.some((at) => b.times.some((bt) => hasScheduleConflict(at, bt)))
      ) {
        conflicts.push([a, b]);
      }
    }),
  );

  const invalidPlan = selected.filter(
    (item) => evaluation?.subjects[item.subjectId]?.state !== "available",
  );

  const planHours = selected.reduce(
    (total, item) =>
      total +
      (catalog?.subjects.find((s) => s.id === item.subjectId)?.hours || 0),
    0,
  );

  const title = subject
    ? subject.name
    : {
        overview: "Seu progresso começa na matriz",
        matrix: "Sua matriz, seu progresso",
        history: "Histórico de conclusões",
        plan: "Planejar semestre",
        saved: "Tudo pronto para revisar",
        profile:
          profile === null ? "Falta só um passo" : "Seu perfil acadêmico",
      }[route] || "Página não encontrada";

  async function toggleCompleted() {
    if (!subject || !completed) return;

    const done = completed.includes(subject.id);

    if (done && !confirmRemoval) {
      setConfirmRemoval(true);
      return;
    }

    await perform(async () => {
      const summary = await academicApi.setCompleted(subject.id, !done);
      historyVersion.current++;
      setEvaluation(summary);
      setCompleted(summary.completed);
      setConfirmRemoval(false);
      setNotice(
        done
          ? "Conclusão removida. Revise os planos afetados."
          : "Conclusão registrada. Seu histórico foi atualizado.",
      );
    });
  }

  async function savePlan() {
    if (
      conflicts.length ||
      invalidPlan.length ||
      missingOfferings.length ||
      !evaluation
    )
      return;

    await perform(async () => {
      await planStorage.save(user.uid, plan);
      setSavedPlan([...plan]);
      go("saved");
    });
  }

  function addOffering(id: string) {
    const offering = catalog?.offerings.find((item) => item.id === id);

    if (!offering) return;

    setPlan((previous) => [
      ...previous.filter(
        (old) =>
          catalog?.offerings.find((item) => item.id === old)?.subjectId !==
          offering.subjectId,
      ),
      id,
    ]);
    setChooser(false);
  }

  async function saveProfile(course: string, period: number) {
    await perform(async () => {
      const result = await academicApi.saveProfile({ course, period });
      setProfile(result.profile);
      setCatalog(undefined);
      setEvaluation(undefined);
      setRetry((value) => value + 1);
      go("overview");
      setNotice("Perfil salvo com sucesso.");
    });
  }

  function retryLoad() {
    if (
      dirty &&
      !window.confirm("Recarregar os dados descarta o rascunho. Continuar?")
    )
      return;
    setError("");
    setRetry((value) => value + 1);
  }

  useEffect(() => {
    setConfirmRemoval(false);
    setNotice("");
  }, [route]);

  return {
    route,
    go,
    profile,
    catalog,
    completed,
    evaluation,
    plan,
    savedPlan,
    error,
    notice,
    busy,
    search,
    setSearch,
    confirmRemoval,
    setConfirmRemoval,
    chooser,
    setChooser,
    dirty,
    perform,
    subject,
    state,
    selected,
    missingOfferings,
    conflicts,
    invalidPlan,
    planHours,
    title,
    toggleCompleted,
    savePlan,
    addOffering,
    setPlan,
    saveProfile,
    retryLoad,
  };
}
