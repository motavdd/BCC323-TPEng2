import "./Week.css";
import type { Offering, Curriculum } from "../../services/api";

const days = [
  ["seg", "segunda", "Segunda"],
  ["ter", "terca", "Terça"],
  ["qua", "quarta", "Quarta"],
  ["qui", "quinta", "Quinta"],
  ["sex", "sexta", "Sexta"],
  ["sab", "sabado", "Sábado"],
];

function dayIndex(value: string) {
  const day = value.trim().toLowerCase().replace(/-feira$/, "")
    .normalize("NFD").replace(/\p{Diacritic}/gu, "");
  return days.findIndex(([short, full]) => day === short || day === full);
}

export function Week({
  offerings,
  catalog,
}: {
  offerings: Offering[];
  catalog: Curriculum;
}) {
  const names = new Map(catalog.subjects.map(({ id, name }) => [id, name]));
  const lessons = offerings.flatMap((offering) =>
    offering.times.map((time, index) => ({
      ...time,
      dayIndex: dayIndex(time.day),
      key: JSON.stringify([offering.id, index]),
      name: names.get(offering.subjectId) ?? offering.subjectId,
      group: offering.group,
    })),
  );
  const hours = [...new Set(lessons.filter((lesson) => lesson.dayIndex >= 0)
    .map(({ start }) => start))].sort();
  const unlisted = lessons.filter((lesson) => lesson.dayIndex < 0);

  return (
    <>
      <div className="week-scroll" tabIndex={0} role="region" aria-label="Horários da semana">
        <table className="week" aria-label="Calendário semanal de aulas">
          <thead>
            <tr>
              <th scope="col">Hora</th>
              {days.map(([, , day]) => (
                <th scope="col" key={day}>
                  {day}
                </th>
              ))}
            </tr>
          </thead>
          <tbody>
            {hours.map((hour) => (
              <tr key={hour}>
                <th scope="row">{hour}</th>
                {days.map(([day], index) => (
                  <td key={day}>
                    {lessons.filter((lesson) => lesson.dayIndex === index && lesson.start === hour)
                      .map((lesson) => (
                        <div className="class-slot" key={lesson.key}>
                          <b>{lesson.name}</b>
                          <div>Turma {lesson.group}</div>
                          <div>{lesson.start}–{lesson.end}</div>
                        </div>
                      ))}
                  </td>
                ))}
              </tr>
            ))}
            {!hours.length && (
              <tr><td colSpan={days.length + 1}>Nenhuma aula na semana.</td></tr>
            )}
          </tbody>
        </table>
      </div>
      {unlisted.length > 0 && (
        <>
          <p>Horários fora do calendário de segunda a sábado</p>
          <ul>
            {unlisted.map((lesson) => (
              <li key={lesson.key}>
                {lesson.name} · Turma {lesson.group} · {lesson.day} · {lesson.start}–{lesson.end}
              </li>
            ))}
          </ul>
        </>
      )}
    </>
  );
}
