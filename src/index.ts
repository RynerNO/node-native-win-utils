import { keyDownHandler, keyUpHandler } from "node-native-win-utils";
import EventEmitter from "events";
import keyCodes from "./keyCodes";

export class KeyListener extends EventEmitter {
  constructor() {
    super();
    keyDownHandler((keyCode) => {
      this.emit("keydown", {
        keyCode,
        keyName: keyCodes[keyCode],
      });
    });
    keyUpHandler((keyCode) => {
      this.emit("keyup", {
        keyCode,
        keyName: keyCodes[keyCode],
      });
    });
  }
}
