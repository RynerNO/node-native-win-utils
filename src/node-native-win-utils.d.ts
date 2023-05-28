declare module "node-native-win-utils" {
  export type WindowData = {
    width: number;
    height: number;
    x: number;
    y: number;
  };

  export function getWindowData(windowName: string): WindowData;
  export function captureWindow(windowName: string, outputPath: string): void;
  export function keyDownHandler(callback: (keyCode: number) => void): void;
  export function keyUpHandler(callback: (keyCode: number) => void): void;
  export class KeyListener {
    constructor();
    on(
      event: "keydown",
      callback: (data: { keyCode: number; keyName: string }) => void
    ): void;
    on(
      event: "keyup",
      callback: (data: { keyCode: number; keyName: string }) => void
    ): void;
  }
}
