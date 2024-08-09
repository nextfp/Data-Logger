import { useRef, useState, useEffect } from "react";
import {
  RealtimeDatabaseClientType,
  RealtimeDatabase,
  onlineStatusType,
  databaseType,
} from "./lib/firebase/firebase";
import { Card } from "./components/ui/card";

function App() {
  const [onlineStatus, setOnlineStatus] = useState<onlineStatusType>({
    time: 0,
    bleStatus: false,
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
    realtimeDatabase.current.getOnlineStatus((data: onlineStatusType) => {
      setOnlineStatus(data);
    });

    realtimeDatabase.current.getDataValue((data: databaseType) => {
      setMachineData(data);
      console.log(data);
    });
  }, []);
  return (
    <section className="grid grid-cols-6 mt-7">
      <div className=" col-span-4 col-start-2">
        <div className="flex">
          <p>インジケータースマホ : </p>
          {new Date().getTime() - onlineStatus.time < 4000 ? (
            <p>オンライン</p>
          ) : (
            <p>オフライン</p>
          )}
        </div>
        <div className="flex">
          <p>インジケータースマホとマイコンの接続 : </p>{" "}
          {onlineStatus.bleStatus ? <p>接続中</p> : <p>接続失敗</p>}
        </div>
        <div className="flex flex-col gap-6">
          <Card>
            <div className="flex gap-6 items-stretch p-2 mx-auto w-fit">
              <div className="flex flex-col gap-3">
                <div className="mx-auto">
                  水温
                  {machineData.temperature}℃
                </div>
                <Card>
                  <div className="mx-auto w-fit">{machineData.speed}km</div>
                  <div className="mx-auto w-fit">gear:{machineData.gear}</div>
                </Card>
                <Card className="h-20">
                  <img
                    className="mx-auto"
                    style={{
                      transform: `rotate(${machineData.angle}deg)`,
                      transformOrigin: "center center",
                      width: "100px",
                    }}
                    src="https://nextfp.github.io/note/cdn/SteeringWheel.png"
                    alt=""
                  />
                </Card>
              </div>
              <Card className="flex p-2">
                <div className=" w-[140px] self-end">
                  <div className="relative flex aspect-[2] items-center justify-center overflow-hidden rounded-t-full bg-[#3CC08E]">
                    <div
                      className={`absolute top-0 aspect-square w-full bg-gradient-to-tr from-transparent from-50% to-white to-50% transition-transform duration-500`}
                      style={{
                        transform: `rotate(${
                          (machineData.rpm / 10000) * 180
                        }deg)`,
                      }}
                    ></div>
                    <div className="absolute top-1/4 flex aspect-square w-3/4 justify-center rounded-full bg-blue-100"></div>
                    <div className="absolute bottom-0 w-full truncate text-center leading-none text-black">
                      {machineData.rpm}rpm
                    </div>
                  </div>
                </div>
              </Card>
              <div>
                <p>緯度:{machineData.latitude}</p>
                <p>経度:{machineData.longitude}</p>
                <p>向き:{machineData.heading}</p>
              </div>
            </div>
          </Card>
        </div>
      </div>
    </section>
  );
}

export default App;
