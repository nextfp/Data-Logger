import { useEffect, useState, useRef } from "react";
import { BleType, machineStatusType, BleController } from "./ble";
import { gpsStatusType, startGPS } from "./gps";
import { Toaster, toast } from "sonner";
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
  const [connectionMemory, setConnectionMemory] = useState<boolean>(false);

  const realtimeDatabase = useRef<RealtimeDatabaseHostType>(
    new RealtimeDatabase()
  );

  const ble = useRef<BleType>(
    new BleController((machineStatus: machineStatusType) => {
      setMachineStatus(machineStatus);
    })
  );
  useEffect(() => {
    const intervalId1 = setInterval(async () => {
      const status = ble.current.getConnectionStatus();
      realtimeDatabase.current.updateOnlineStatus(status);
      if (!status) {
        ble.current.reconnect();
      }
      if (status !== connectionMemory) {
        setConnectionMemory(status);
        if (status) {
          toast.success("マイコンと接続されました。");
        } else {
          toast.error("マイコンとの接続が切れました。");
        }
      }
    }, 1000);

    const intervalId2 = setInterval(async () => {
      const SDFilePath = new Date().toLocaleDateString("sv-SE");
      await ble.current.sendData(
        `${SDFilePath},${new Date().getTime()},${gpsStatus.latitude},${
          gpsStatus.longitude
        },${gpsStatus.speed},${gpsStatus.heading}`
      );
    }, 300);

    const intervalId3 = setInterval(async () => {
      realtimeDatabase.current.send({
        latitude: gpsStatus.latitude,
        longitude: gpsStatus.longitude,
        speed: gpsStatus.speed,
        heading: gpsStatus.heading,
        gear: machineStatus.gear,
        rpm: machineStatus.rpm,
        temperature: machineStatus.temperature,
        angle: machineStatus.angle,
      });
    }, 100);

    return () => {
      clearInterval(intervalId1);
      clearInterval(intervalId2);
      clearInterval(intervalId3);
    };
  }, [connectionMemory, machineStatus, gpsStatus]);

  const clickConnect = () => {
    ble.current.firstConnect();
    startGPS((gpsStatus: gpsStatusType) => {
      setGpsStatus(gpsStatus);
    });
  };

  return (
    <main className="mx-auto w-fit flex flex-col gap-4 mt-5">
      <Toaster position="top-right" />
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
      {!connectionMemory && (
        <p className="mx-auto">マイコンと接続されていません。</p>
      )}
    </main>
  );
}
