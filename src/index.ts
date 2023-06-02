import EventEmitter = require("events");
import path from "path";

const bindings = require("node-gyp-build")(path.resolve(__dirname, ".."));

import keyCodes from "./keyCodes";

type WindowData = {
  width: number;
  height: number;
  x: number;
  y: number;
};
type ImageData = {
  width: number;
  height: number;
  data: Uint8Array;
};
type MatchData = {
  minValue: number;
  maxValue: number;
  minLocation: { x: number; y: number };
  maxLocation: { x: number; y: number };
};

export type GetWindowData = (windowName: string) => WindowData;

export type CaptureWindow = (windowName: string, outputPath: string) => void;

export type KeyDownHandler = (callback: (keyCode: number) => void) => void;

export type KeyUpHandler = (callback: (keyCode: number) => void) => void;

export type MouseMove = (posX: number, posY: number) => boolean;

export type MouseClick = (button?: "left" | "middle" | "right") => boolean;

export type TypeString = (stringToType: string, delay?: number) => boolean;

export type MouseDrag = (
  starX: number,
  startY: number,
  endX: Number,
  endY: number,
  speed?: number
) => boolean;

export type Imread = (path: string) => ImageData;

export type Imwrite = (path: string, image: ImageData) => ImageData;

export type MatchTemplate = (
  image: ImageData,
  template: ImageData,
  method?: number | null,
  mask?: ImageData
) => ImageData;

export type Blur = (
  image: ImageData,
  sizeX: number,
  sizeY: number
) => ImageData;
export type BgrToGray = (image: ImageData) => ImageData;

export interface KeyListener extends EventEmitter {
  on(
    event: "keyDown",
    callback: (data: { keyCode: number; keyName: string }) => void
  ): this;
  on(
    event: "keyUp",
    callback: (data: { keyCode: number; keyName: string }) => void
  ): this;
}

const {
  keyDownHandler,
  keyUpHandler,
  getWindowData,
  captureWindow,
  mouseMove,
  mouseClick,
  mouseDrag,
  typeString,
  imread,
  imwrite,
  matchTemplate,
  blur,
  bgrToGray,
}: {
  keyDownHandler: KeyDownHandler;
  keyUpHandler: KeyUpHandler;
  getWindowData: GetWindowData;
  captureWindow: CaptureWindow;
  mouseMove: MouseMove;
  mouseClick: MouseClick;
  mouseDrag: MouseDrag;
  typeString: TypeString;
  imread: Imread;
  imwrite: Imwrite;
  matchTemplate: MatchTemplate;
  blur: Blur;
  bgrToGray: BgrToGray;
} = bindings;
export class KeyListener extends EventEmitter {
  constructor() {
    super();
    keyDownHandler((keyCode: number) => {
      const keyName: string | undefined = keyCodes.get(keyCode.toString());
      this.emit("keyDown", {
        keyCode,
        keyName,
      });
    });
    keyUpHandler((keyCode: number) => {
      const keyName: string | undefined = keyCodes.get(keyCode.toString());
      this.emit("keyUp", {
        keyCode,
        keyName,
      });
    });
  }
}
const opencv = {
  imread,
  imwrite,
  matchTemplate,
  blur,
  bgrToGray,
};
export {
  keyDownHandler,
  keyUpHandler,
  getWindowData,
  captureWindow,
  mouseMove,
  mouseClick,
  mouseDrag,
  typeString,
  opencv,
};
