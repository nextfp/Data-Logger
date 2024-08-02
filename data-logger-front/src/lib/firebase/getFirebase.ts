import { initializeApp, FirebaseOptions, FirebaseApp } from "firebase/app";
import { Database, getDatabase } from "firebase/database";

const firebaseConfig: FirebaseOptions = {
  apiKey: "AIzaSyC_cMiijXqV1neIMOxgcy88vQSTzHMxlrs",
  authDomain: "data-logger-back.firebaseapp.com",
  databaseURL: "https://data-logger-back-default-rtdb.firebaseio.com",
  projectId: "data-logger-back",
  storageBucket: "data-logger-back.appspot.com",
  messagingSenderId: "773704317420",
  appId: "1:773704317420:web:71b1d8f8935958f480a8c9",
};

let app: FirebaseApp | undefined;
let database: Database | undefined;

const getFirebaseApp = (): FirebaseApp => {
  if (!app) {
    app = initializeApp(firebaseConfig);
  }
  return app;
};

export const getFirebaseDatabase = () => {
  if (!database) {
    console.log(firebaseConfig.databaseURL);
    database = getDatabase(getFirebaseApp());
  }
  return database;
};
