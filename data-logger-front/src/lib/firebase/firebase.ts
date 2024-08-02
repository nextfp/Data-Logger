import { Database, ref, set } from "firebase/database";
import { getFirebaseDatabase } from "./getFirebase";

export interface RealtimeDatabaseHostType {
  send: (data: databaseType) => void;
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

export class RealtimeDatabase implements RealtimeDatabaseHostType {
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
    const dbPath = this.path + "/" + this.getTIme();
    set(ref(this.database, dbPath), {
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
}
