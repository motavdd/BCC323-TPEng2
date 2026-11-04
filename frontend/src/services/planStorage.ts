import { doc, getDoc, serverTimestamp, setDoc } from "firebase/firestore";
import { db } from "./firebase";

// O plano usa o armazenamento já existente; os dados acadêmicos usam academic.ts.
export const planStorage = {
  async load(userId: string): Promise<string[]> {
    const saved = await getDoc(doc(db, "users", userId, "plans", "next"));
    return saved.exists() ? (saved.data().offeringIds as string[]) : [];
  },

  async save(userId: string, offeringIds: string[]): Promise<void> {
    await setDoc(doc(db, "users", userId, "plans", "next"), {
      offeringIds,
      updatedAt: serverTimestamp(),
    });
  },
};
