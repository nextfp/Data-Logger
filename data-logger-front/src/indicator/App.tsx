import { useState } from "react";
import { BleType, machineStatusType, BleController } from "./ble";
import { gpsStatusType, startGPS } from "./gps";
import {
  RealtimeDatabaseHostType,
  RealtimeDatabase,
} from "../lib/firebase/firebase";

export default function App() {
  const [machineStatus, setMachineStatus] = useState<machineStatusType>({
    gear: -1,
    rpm: -1,
    temperature: -1,
    angle: 0,
  });
  const [gpsStatus, setGpsStatus] = useState<gpsStatusType>({
    latitude: 0,
    longitude: 0,
    speed: 0,
    heading: 0,
  });

  const realtimeDatabase: RealtimeDatabaseHostType = new RealtimeDatabase();

  const ble: BleType = new BleController((machineStatus: machineStatusType) => {
    setMachineStatus(machineStatus);
    realtimeDatabase.send({
      latitude: gpsStatus.latitude,
      longitude: gpsStatus.longitude,
      speed: gpsStatus.speed,
      heading: gpsStatus.heading,
      gear: machineStatus.gear,
      rpm: machineStatus.rpm,
      temperature: machineStatus.temperature,
      angle: machineStatus.angle,
    });
  });

  const clickConnect = () => {
    ble.firstConnect();
    startGPS((gpsStatus: gpsStatusType) => {
      setGpsStatus(gpsStatus);
    });
  };

  return (
    <main className="mx-auto w-fit flex flex-col gap-4 mt-5">
      <div className="flex gap-2 w-fit mx-auto">
        <button
          className="bg-black dark:bg-white hover:bg-blue-700 text-white dark:text-black font-bold py-1 px-2 rounded"
          type="button"
          id="connect-ble"
          onClick={clickConnect}
        >
          connect-ble
        </button>
      </div>
      <div className="grid grid-cols-4 gap-6">
        <div className="border border-black dark:border-white rounded-3xl p-2 h-32 w-40">
          <p className="mt-0 h-fit">Gear</p>
          <p className="mx-auto w-fit text-6xl font-bold mt-2" id="gear">
            {machineStatus.gear}
          </p>
        </div>

        <div className="border border-black rounded-3xl p-2 h-32 w-40">
          <p className="mt-0 h-fit">Speed</p>
          <p className="mx-auto w-fit text-6xl font-bold mt-2" id="speed">
            {gpsStatus.speed}
          </p>
        </div>

        <div className="border border-black rounded-3xl p-2 h-32 w-40">
          <p className="mt-0 h-fit">RPM</p>
          <p className="mx-auto w-fit text-6xl font-bold mt-2" id="RPM">
            {machineStatus.rpm}
          </p>
        </div>
        <div className="border border-black rounded-3xl p-2 h-32 w-40">
          <p className="mt-0 h-fit">水温</p>
          <p className="mx-auto w-fit text-6xl font-bold mt-2" id="temperature">
            {machineStatus.temperature}
          </p>
        </div>
      </div>
    </main>
  );
}
