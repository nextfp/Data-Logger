export type gpsStatusType = {
  latitude: number;
  longitude: number;
  heading: number;
  speed: number;
};

const options = {
  enableHighAccuracy: true, // 位置情報の精度を高める
  timeout: 100, // タイムアウトまでの時間（ミリ秒）
  maximumAge: 0, // キャッシュされた位置情報の最大年齢
};

export const startGPS = (setCallbacks: (gpsStatus: gpsStatusType) => void) => {
  navigator.geolocation.watchPosition(
    (position) => {
      let { latitude, longitude, speed, heading } = position.coords;
      speed = speed ? Math.floor((speed * 3600) / 1000) : 0;
      heading = heading ? heading : -1;
      const data: gpsStatusType = {
        latitude,
        longitude,
        speed,
        heading,
      };
      setCallbacks(data);
    },
    null,
    options
  );
};
