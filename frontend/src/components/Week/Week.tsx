import "./Week.css";
import type { Offering, Curriculum } from "../../services/api";
export function Week({
  offerings,
  catalog,
}: {
  offerings: Offering[];
  catalog: Curriculum;
}) {
  return (
    <div className="week-scroll">
      <table className="week">
        <thead>
          <tr>
            <th scope="col">Hora</th>
            {["Seg", "Ter", "Qua", "Qui", "Sex"].map((day) => (
              <th scope="col" key={day}>
                {day}
              </th>
            ))}
          </tr>
        </thead>
        <tbody>
          {["08:00", "10:00", "12:00", "14:00", "16:00", "18:00"].map(
            (hour) => (
              <tr key={hour}>
                <th scope="row">{hour}</th>
                {["Seg", "Ter", "Qua", "Qui", "Sex"].map((day) => (
                  <td key={day}>
                    {offerings.flatMap((item) =>
                      item.times
                        .filter(
                          (time) => time.day === day && time.start === hour,
                        )
                        .map((time) => (
                          <div className="class-slot" key={item.id}>
                            <b>
                              {
                                catalog.subjects.find(
                                  (s) => s.id === item.subjectId,
                                )?.name
                              }
                            </b>
                            <br />
                            {time.start}–{time.end}
                          </div>
                        )),
                    )}
                  </td>
                ))}
              </tr>
            ),
          )}
        </tbody>
      </table>
    </div>
  );
}
