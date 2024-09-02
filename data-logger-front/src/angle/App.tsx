import { useEffect, useState, useRef } from "react";
import {
  RealtimeDatabaseClientType,
  RealtimeDatabase,
  databaseType,
} from "../lib/firebase/firebase";
import { Button } from "../components/ui/button";

export default function App() {
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
    realtimeDatabase.current.getDataValue((data: databaseType) => {
      setMachineData(data);
      console.log(data);
    });

    return () => {};
  }, []);

  return (
    <main className="flex justify-center mt-10">
      <div className="flex flex-col justify-center gap-20">
        <h1 className="mx-auto">現在の舵角センサーの角度:</h1>
        <h2
          className={`w-60 border-b-black border-b-2 text-center`}
          style={{
            transform: `rotate(${machineData.angle}deg)`,
          }}
        >
          {machineData.angle}
        </h2>
        <Button
          onClick={() => {
            realtimeDatabase.current.setAngleCallib(machineData.angle);
          }}
        >
          現在の角度を0度に設定
        </Button>
      </div>
    </main>
  );
}
