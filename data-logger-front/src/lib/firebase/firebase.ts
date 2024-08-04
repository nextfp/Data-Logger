import { Database, onValue, ref, set } from "firebase/database";
import { getFirebaseDatabase } from "./getFirebase";

export interface RealtimeDatabaseHostType {
  send: (data: databaseType) => void;
  updateOnlineStatus: (bleStatus: boolean) => void;
}

export interface RealtimeDatabaseClientType {
  getDataValue: (callbacks: (data: databaseType) => void) => void;
  getOnlineStatus: (callbacks: (data: onlineStatusType) => void) => void;
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
  time: number;
  bleStatus: boolean;
};

export class RealtimeDatabase
  implements RealtimeDatabaseHostType, RealtimeDatabaseClientType
{
  private database: Database;
  private path: string;

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
    const dbRef = ref(this.database, "realtime");
    onValue(dbRef, (snapshot) => {
      const data = snapshot.val();
      if (data) {
        callbacks(data);
      }
    });
  };

  getOnlineStatus = (callbacks: (data: onlineStatusType) => void) => {
    const dbRef = ref(this.database, "online");
    onValue(dbRef, (snapshot) => {
      const data = snapshot.val();
      if (data) {
        callbacks(data);
      }
    });
  };
}
