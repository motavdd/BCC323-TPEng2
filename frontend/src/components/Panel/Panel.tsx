import "./Panel.css";
import type { ReactNode } from "react";
export function Panel({
  children,
  tone = "",
}: {
  children: ReactNode;
  tone?: string;
}) {
  return <section className={`panel ${tone}`}>{children}</section>;
}
