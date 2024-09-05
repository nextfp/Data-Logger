import { useRef, useState, useEffect } from "react";
import {
  RealtimeDatabaseClientType,
  RealtimeDatabase,
  onlineStatusType,
  databaseType,
} from "./lib/firebase/firebase";
import { LogViewer } from "./components/logviewer";

function App() {
  const [onlineStatus, setOnlineStatus] = useState<onlineStatusType>({
    indicator: false,
    ble: false,
  });
  const [machineData, setMachineData] = useState<databaseType>({
    latitude: 0,
    longitude: 0,
    speed: 0,
    heading: 0,
    gear: 0,
    rpm: 0,
    temperature: 0,
    angle: 0,
  });

  const realtimeDatabase = useRef<RealtimeDatabaseClientType>(
    new RealtimeDatabase()
  );

  useEffect(() => {
    let onlineInterval = setInterval(() => {
      realtimeDatabase.current.getOnlineStatus((data: onlineStatusType) => {
        setOnlineStatus(data);
      });
    }, 1000);

    realtimeDatabase.current.getDataValue((data: databaseType) => {
      setMachineData(data);
      console.log(data);
    });

    return () => {
      clearInterval(onlineInterval);
    };
  }, []);
  return (
    <main>
      <a href="/log/" className="text-blue-400">
        過去のログの閲覧はこちら
      </a>
      <section className="grid grid-cols-6 mt-7">
        <div className=" col-span-4 col-start-2">
          <div className="flex">
            <p>インジケータースマホ : </p>
            {onlineStatus.indicator ? <p>オンライン</p> : <p>オフライン</p>}
          </div>
          <div className="flex">
            <p>インジケータースマホとマイコンの接続 : </p>{" "}
            {onlineStatus.ble ? <p>接続中</p> : <p>接続失敗</p>}
          </div>
          <LogViewer machineData={machineData} />
        </div>
      </section>
    </main>
  );
}

export default App;
