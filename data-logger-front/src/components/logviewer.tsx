import { databaseType } from "../lib/firebase/firebase";
import { Card } from "./ui/card";

export const LogViewer = ({ machineData }: { machineData: databaseType }) => (
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
                  transform: `rotate(${(machineData.rpm / 10000) * 180}deg)`,
                }}
              ></div>
              <div className="absolute top-1/4 flex aspect-square w-3/4 justify-center rounded-full bg-blue-100"></div>
              <div className="absolute bottom-0 w-full truncate text-center leading-none text-black">
                {machineData.rpm}rpm
              </div>
            </div>
          </div>
        </Card>
      </div>
    </Card>
    <Card>
      <div>
        <p>緯度:{machineData.latitude}</p>
        <p>経度:{machineData.longitude}</p>
        <p>向き:{machineData.heading}</p>
      </div>
      <div>
        <Card className="h-[700px]">
          <div
            className=" bg-red-600 h-2 w-2 rounded-full"
            style={{
              transform: `translateX(${
                (machineData.latitude - 38.009685) * 1000000
              }px) translateY(${
                (machineData.longitude - 139.491804) * 1000000
              }px) rotate(${machineData.heading}deg)`,
            }}
          />
        </Card>
      </div>
    </Card>
  </div>
);
