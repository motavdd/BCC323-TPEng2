import { useEffect, useState } from "react";

function readRoute() {
  return window.location.hash.slice(1) || "overview";
}

export function usePlannerRoute() {
  const [route, setRoute] = useState(readRoute);
  useEffect(() => {
    const change = () => setRoute(readRoute());
    window.addEventListener("hashchange", change);
    return () => window.removeEventListener("hashchange", change);
  }, []);

  function go(next: string) {
    window.location.hash = next;
  }

  return { route, go };
}
