import { Database, onValue, ref, set, get, child } from "firebase/database";
import { getFirebaseDatabase } from "./getFirebase";

export interface RealtimeDatabaseHostType {
  send: (data: databaseType) => void;
  updateOnlineStatus: (bleStatus: boolean) => void;
}

export interface RealtimeDatabaseClientType {
  getDataValue: (callbacks: (data: databaseType) => void) => void;
  getOnlineStatus: (callbacks: (data: onlineStatusType) => void) => void;
  setAngleCallib: (angle: number) => Promise<void>;
}

export type databaseType = {
  latitude: number;
  longitude: number;
  speed: number;
  heading: number;
  gear: number;
  rpm: number;
  temperature: number;
  angle: number;
};

export type onlineStatusType = {
  indicator: boolean;
  ble: boolean;
};

export class RealtimeDatabase
  implements RealtimeDatabaseHostType, RealtimeDatabaseClientType
{
  private database: Database;
  private path: string;
  private angleCallib: number = 0;

  constructor() {
    this.database = getFirebaseDatabase();
    this.path = new Date().toLocaleDateString("sv-SE");
  }

  private getTIme = (): string => {
    return new Date().toISOString().replace(".", ":").replace("Z", "");
  };

  send: (data: databaseType) => void = (data: databaseType) => {
    const logPath = this.path + "/" + this.getTIme();
    set(ref(this.database, logPath), {
      latitude: data.latitude,
      longitude: data.longitude,
      speed: data.speed,
      heading: data.heading,
      gear: data.gear,
      rpm: data.rpm,
      temperature: data.temperature,
      angle: data.angle,
    });
    const realtimePath = "realtime";
    set(ref(this.database, realtimePath), {
      latitude: data.latitude,
      longitude: data.longitude,
      speed: data.speed,
      heading: data.heading,
      gear: data.gear,
      rpm: data.rpm,
      temperature: data.temperature,
      angle: data.angle,
    });
  };

  updateOnlineStatus: (bleStatus: boolean) => void = (bleStatus: boolean) => {
    set(ref(this.database, "online"), {
      time: new Date().getTime(),
      bleStatus: bleStatus,
    });
  };

  getDataValue: (callbacks: (data: databaseType) => void) => void = (
    callbacks: (data: databaseType) => void
  ) => {
    const angleDbRef = ref(this.database, "angle");
    onValue(angleDbRef, (snapshot) => {
      const data = snapshot.val();
      if (data) {
        this.angleCallib = data.angle;
      } else {
        this.angleCallib = 0;
      }
    });
    const machineDbRef = ref(this.database, "realtime");
    onValue(machineDbRef, (snapshot) => {
      const data = snapshot.val();
      if (data) {
        data.angle = data.angle - this.angleCallib;
        callbacks(data);
      }
    });
  };

  getOnlineStatus = (callbacks: (data: onlineStatusType) => void) => {
    const dbRef = ref(this.database, "online");
    get(child(dbRef, "/")).then((snapshot) => {
      const data = snapshot.val();
      if (data) {
        //const indicator = new Date().getTime() - data.time < 5000;
        const indicator = new Date().getTime() - data.time < 5000;

        const ble: boolean = indicator && data.bleStatus;
        const status = { indicator: indicator, ble: ble };
        console.log("status", status);
        callbacks(status);
      }
    });
  };

  setAngleCallib: (angle: number) => Promise<void> = async () => {
    const db = getFirebaseDatabase();
    const dbRef = ref(db, "angle");
    await set(dbRef, {
      time: new Date().getTime(),
      angle: await this.angle,
    });
  };

  get angle(): Promise<number> {
    const machineDbRef = ref(this.database, "realtime");
    return get(child(machineDbRef, "/")).then((snapshot) => {
      const data = snapshot.val();
      if (data) {
        return data.angle;
      } else {
        console.error("angle is not found");
        return 0;
      }
    });
  }
}
