import "./Week.css";
import type { Offering, Curriculum } from "../../services/api";
export function Week({
  offerings,
  catalog,
}: {
  offerings: Offering[];
  catalog: Curriculum;
}) {
  const days = ["Seg", "Ter", "Qua", "Qui", "Sex", "Sáb"];
  const hours = [...new Set([
    "08:00", "10:00", "12:00", "14:00", "16:00", "18:00",
    ...offerings.flatMap((item) => item.times.map((time) => time.start)),
  ])].sort();
  return (
    <div className="week-scroll">
      <table className="week">
        <thead>
          <tr>
            <th scope="col">Hora</th>
            {days.map((day) => (
              <th scope="col" key={day}>
                {day}
              </th>
            ))}
          </tr>
        </thead>
        <tbody>
          {hours.map(
            (hour) => (
              <tr key={hour}>
                <th scope="row">{hour}</th>
                {days.map((day) => (
                  <td key={day}>
                    {offerings.flatMap((item) =>
                      item.times
                        .filter(
                          (time) => time.day === day && time.start === hour,
                        )
                        .map((time) => (
                          <div className="class-slot" key={item.id + time.end + time.room}>
                            <b>
                              {
                                catalog.subjects.find(
                                  (s) => s.id === item.subjectId,
                                )?.name
                              }
                            </b>
                            <br />
                            {time.start}–{time.end}
                            {time.room && <div>Sala {time.room}</div>}
                            {item.professor && <div>{item.professor}</div>}
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
