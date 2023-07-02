import EventEmitter = require("events");
import path from "path";
import fs from "fs";

const bindings = require("node-gyp-build")(path.resolve(__dirname, ".."));

import keyCodes from "./keyCodes";

export type WindowData = {
  width: number;
  height: number;
  x: number;
  y: number;
};
export type ImageData = {
  width: number;
  height: number;
  data: Uint8Array;
};
export type MatchData = {
  minValue: number;
  maxValue: number;
  minLocation: { x: number; y: number };
  maxLocation: { x: number; y: number };
};

export type GetWindowData = (windowName: string) => WindowData;

export type CaptureWindow = (windowName: string) => Buffer;

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

export type Point = [x: number, y: number];
export type Color = [r: number, g: number, b: number];
export type ROI = [x: number, y: number, width: number, height: number];
export type Imread = (path: string) => ImageData;

export type Imwrite = (image: ImageData) => Buffer;

export type MatchTemplate = (
  image: ImageData,
  template: ImageData,
  method?: number | null,
  mask?: ImageData
) => MatchData;

export type Blur = (
  image: ImageData,
  sizeX: number,
  sizeY: number
) => ImageData;
export type BgrToGray = (image: ImageData) => ImageData;

export type DrawRectangle = (
  image: ImageData,
  start: Point,
  end: Point,
  rgb: Color,
  thickness: number
) => ImageData;
export type GetRegion = (image: ImageData, region: ROI) => ImageData;
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
  captureWindowN,
  mouseMove,
  mouseClick,
  mouseDrag,
  typeString,
  imread,
  imwrite,
  matchTemplate,
  blur,
  bgrToGray,
  drawRectangle,
  getRegion,
}: {
  keyDownHandler: KeyDownHandler;
  keyUpHandler: KeyUpHandler;
  getWindowData: GetWindowData;
  captureWindowN: CaptureWindow;
  mouseMove: MouseMove;
  mouseClick: MouseClick;
  mouseDrag: MouseDrag;
  typeString: TypeString;
  imread: Imread;
  imwrite: Imwrite;
  matchTemplate: MatchTemplate;
  blur: Blur;
  bgrToGray: BgrToGray;
  drawRectangle: DrawRectangle;
  getRegion: GetRegion;
} = bindings;

function captureWindow(windowName: string, path: string): boolean {
  const buffer = captureWindowN(windowName);
  if (!buffer) return false;
  fs.writeFileSync(path, buffer);
  return true;
}

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

export class OpenCV {
  imageData: ImageData;

  constructor(image: string | ImageData) {
    if (typeof image === "string") {
      this.imageData = imread(image);
    } else {
      this.imageData = image;
    }
  }
  get width() {
    return this.imageData.width;
  }
  get height() {
    return this.imageData.height;
  }

  matchTemplate(template: ImageData, method?: number | null, mask?: ImageData) {
    return matchTemplate(this.imageData, template, method, mask);
  }

  blur(sizeX: number, sizeY: number) {
    return new OpenCV(blur(this.imageData, sizeX, sizeY));
  }

  bgrToGray() {
    return new OpenCV(bgrToGray(this.imageData));
  }

  drawRectangle(start: Point, end: Point, rgb: Color, thickness: number) {
    return new OpenCV(
      drawRectangle(this.imageData, start, end, rgb, thickness)
    );
  }
  getRegion(region: ROI) {
    return new OpenCV(getRegion(this.imageData, region));
  }
  imwrite(path: string) {
    const buffer = imwrite(this.imageData);
    if (!buffer) return;
    fs.writeFileSync(path, buffer);
  }
}

export {
  keyDownHandler,
  keyUpHandler,
  getWindowData,
  captureWindow,
  captureWindowN,
  mouseMove,
  mouseClick,
  mouseDrag,
  typeString,
};
