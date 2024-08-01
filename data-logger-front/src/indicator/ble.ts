export interface BleType {
  firstConnect: () => void;
  sendData: (data: string) => void;
}

export type machineStatusType = {
  gear: number;
  rpm: number;
  temperature: number;
  angle: number;
};

export class BleController implements BleType {
  private kUARTServiceUUID = "4fafc201-1fb5-459e-8fcc-c5c9c331914b";
  private kTXCharacteristicUUID = "beb5483e-36e1-4688-b7f5-ea07361b26a9";
  private kRXCharacteristicUUID = "beb5483f-36e1-4688-b7f5-ea07361b26a8";
  private device: BluetoothDevice | undefined;
  private server: BluetoothRemoteGATTServer | undefined;
  private service: BluetoothRemoteGATTService | undefined;
  private characteristicRx: BluetoothRemoteGATTCharacteristic | undefined;
  private characteristicTx: BluetoothRemoteGATTCharacteristic | undefined;

  private readCallback: (data: machineStatusType) => void;

  constructor(readCallback: (data: machineStatusType) => void) {
    this.readCallback = readCallback;
  }

  firstConnect: () => void = async () => {
    try {
      this.device = await navigator.bluetooth.requestDevice({
        filters: [{ services: [this.kUARTServiceUUID] }],
      });
      this.device.addEventListener("gattserverdisconnected", () => {
        this.initBle();
      });
      this.initBle();
    } catch (error) {
      console.error("device:" + error);
    }
  };

  initProperties = () => {
    this.server = undefined;
    this.service = undefined;
    this.characteristicRx = undefined;
    this.characteristicTx = undefined;
  };

  initBle = async () => {
    if (this.device) {
      try {
        this.server = await this.device.gatt?.connect();
        this.service = await this.server?.getPrimaryService(
          this.kUARTServiceUUID
        );
        this.characteristicTx = await this.service?.getCharacteristic(
          this.kTXCharacteristicUUID
        );
        this.characteristicRx = await this.service?.getCharacteristic(
          this.kRXCharacteristicUUID
        );
        this.readData();
      } catch (error) {
        console.error("BLE Initialization error:", error);
      }
    }
  };

  readData = async () => {
    if (this.characteristicRx) {
      this.characteristicRx.addEventListener(
        "characteristicvaluechanged",
        (event) => {
          const value = new TextDecoder().decode(
            (event.target as BluetoothRemoteGATTCharacteristic).value
          );
          console.log("readData: " + value);
          let [gear, rpm, temperature, angle] = value.split(",").map(Number);
          rpm = Math.floor(rpm * 10) / 10;
          const machineStatus: machineStatusType = {
            gear,
            rpm,
            temperature,
            angle,
          };
          this.readCallback(machineStatus);
        }
      );

      await this.characteristicRx.startNotifications();
    } else {
      console.error("characteristicRx is not initialized");
    }
  };

  sendData: (data: string) => void = (data: string) => {
    const encoder = new TextEncoder();
    const encodeData = encoder.encode(data);
    if (this.characteristicTx) {
      this.characteristicTx
        .writeValue(encodeData)
        .then(() => {
          console.log("sendData: " + data);
        })
        .catch((error) => {
          console.error("Error writing value: " + error);
        });
      console.log("sendData: " + data);
    } else {
      console.error("characteristicTx is not initialized");
    }
  };
}
