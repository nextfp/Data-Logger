import { useEffect, useState, useRef } from "react";
import {
  RealtimeDatabaseClientType,
  RealtimeDatabase,
  databaseType,
} from "../lib/firebase/firebase";
import { LogViewer } from "../components/logviewer";
import { Toaster, toast } from "sonner";
import { Button } from "../components/ui/button";

export default function App() {
  const [logData, setlogData] = useState<{
    time: string;
    data: databaseType;
  }>({
    time: "unknown",
    data: {
      latitude: 0,
      longitude: 0,
      speed: 0,
      heading: 0,
      gear: 0,
      rpm: 0,
      temperature: 0,
      angle: 0,
    },
  });

  const [inputData, setInputData] = useState("08-31 04:36:36");

  const [dateTime, setDateTime] = useState("2024-08-31T04:36:36:000");

  const handleInputChange = (event: React.ChangeEvent<HTMLInputElement>) => {
    // handleInputChangeではsetDateTimeを呼び出さない
    setInputData(event.target.value);
  };

  const handleSubmit = (event: React.FormEvent<HTMLFormElement>) => {
    event.preventDefault();
    const date = new Date(
      new Date("2024-" + inputData).getTime() + 9 * 60 * 60 * 1000
    )
      .toISOString()
      .replace(".", ":")
      .replace("Z", "");
    setDateTime(date);
    console.log(dateTime);
  };

  const realtimeDatabase = useRef<RealtimeDatabaseClientType>(
    new RealtimeDatabase()
  );

  useEffect(() => {
    let interval: NodeJS.Timeout;
    realtimeDatabase.current.getlogData("2024-08-31", dateTime).then((data) => {
      if (data) {
        console.log(Object.keys(data));
        let logDatas = Object.keys(data).map((time) => ({
          time,
          data: data[time],
        }));
        if (logDatas.length != 0) {
          interval = setInterval(() => {
            const nextData = logDatas.shift();
            if (nextData) {
              setlogData(nextData);
              //console.log(nextData.time);
            } else {
              logDatas = Object.keys(data).map((time) => ({
                time,
                data: data[time],
              }));
            }
          }, 300);
        }
      } else {
        toast.error("データがありません。");
      }
    });

    return () => {
      clearInterval(interval);
    };
  }, [dateTime, realtimeDatabase]);
  return (
    <>
      <Toaster position="top-right" />
      <main className="flex flex-col gap-6 mt-5">
        <form onSubmit={handleSubmit} className="flex gap-3">
          <label htmlFor="dateTime">見たい日付と時間 (mm-dd hh:mm:ss):</label>
          <input
            className="border border-gray-300 rounded-md"
            type="text"
            id="dateTime"
            name="dateTime"
            value={inputData}
            onChange={handleInputChange}
          />
          <Button type="submit">適用</Button>
        </form>
        <p>
          time:
          {new Date(
            new Date(logData.time.replace("T", " ")).getTime() +
              9 * 60 * 60 * 1000
          ).toLocaleTimeString()}
        </p>
        <LogViewer machineData={logData.data} />
      </main>
    </>
  );
}
