[![License][license-src]][license-href]

# Node Native Win Utils

I did it for myself because I didn't feel like dealing with libraries like 'node-ffi' to implement this functionality. Maybe someone will find it useful. It's WINDOWS OS ONLY

This package is a native addon for Node.js that allows you to perform various utility operations on Windows systems. It includes key event listeners, window data retrieval, window screenshot capture functionality, mouse movement, mouse click, mouse drag, and typing functionality.

## Installation

You can install the package using npm:

```shell
npm install node-native-win-utils
```

## Usage

### Importing the Package

To use the package, import the necessary functions, types, and classes:

```javascript
import {
  keyDownHandler,
  keyUpHandler,
  getWindowData,
  captureWindow,
  mouseMove,
  mouseClick,
  mouseDrag,
  typeString,
  KeyListener,
} from "node-native-win-utils";
```

### Key Event Listeners

The package provides `keyDownHandler` and `keyUpHandler` functions, which allow you to register callbacks for key down and key up events, respectively. The callbacks receive the `keyCode` as a parameter:

```javascript
keyDownHandler((keyCode) => {
  console.log("Key down:", keyCode);
});

// Key down: 8

keyUpHandler((keyCode) => {
  console.log("Key up:", keyCode);
});

// Key up: 8
```

### Window Data

The `getWindowData` function retrieves information about a specific window identified by its name. It returns an object with properties `width`, `height`, `x`, and `y`, representing the window dimensions and position:

```javascript
const windowData = getWindowData("Window Name");

console.log("Window data:", windowData);

// Window data: { width: 800, height: 600, x: 50, y: 50 }
```

### Window Capture

The `captureWindow` function allows you to capture a screenshot of a specific window identified by its name. Provide the window name and the output path as parameters:

```javascript
captureWindow("Window Name", "output.png");

// Output: output.png with a screenshot of the window
```

### Mouse Movement

The `mouseMove` function allows you to move the mouse to a specific position on the screen. Provide the `posX` and `posY` coordinates as parameters:

```javascript
mouseMove(100, 200);
```

### Mouse Click

The `mouseClick` function allows you to perform a mouse click event. Optionally, you can specify the mouse button as a parameter (`"left"`, `"middle"`, or `"right"`). If no button is specified, a left mouse click is performed by default:

```javascript
mouseClick(); // Left mouse click
mouseClick("right"); // Right mouse click
```

### Mouse Drag

The `mouseDrag` function allows you to simulate dragging the mouse from one position to another. Provide the starting and ending coordinates (`startX`, `startY`, `endX`, `endY`) as parameters. Optionally, you can specify the speed at which the mouse should be dragged:

```javascript
mouseDrag(100, 200, 300, 400);
mouseDrag(100, 200, 300, 400, 100); // Drag with speed 100
```

### Typing

The `typeString` function allows you to simulate typing a string of characters. Provide the string to type as the `stringToType` parameter. Optionally, you can specify

a delay between each character (in milliseconds) using the `delay` parameter:

```javascript
typeString("Hello, world!");
typeString("Hello, world!", 100); // Type with a delay of 100ms between characters
```

### Key Listener Class

The `KeyListener` class extends the EventEmitter class and simplifies working with the `keyDownHandler` and `keyUpHandler` functions. You can register event listeners for the "keyDown" and "keyUp" events using the `on` method:

```javascript
const listener = new KeyListener();

listener.on("keyDown", (data) => {
  console.log("Key down:", data.keyCode, data.keyName);
});

// Key down: 8 Backspace

listener.on("keyUp", (data) => {
  console.log("Key up:", data.keyCode, data.keyName);
});

// Key up: 8 Backspace
```

## Functions

| Function       | Parameters                                                                              | Return Type            |
| -------------- | --------------------------------------------------------------------------------------- | ---------------------- |
| keyDownHandler | `callback: (keyCode: number) => void`                                                   | `void`                 |
| keyUpHandler   | `callback: (keyCode: number) => void`                                                   | `void`                 |
| getWindowData  | `windowName: string`                                                                    | `WindowData`           |
| captureWindow  | `windowName: string, outputPath: string`                                                | `void`                 |
| mouseMove      | `posX: number, posY: number`                                                            | `boolean`              |
| mouseClick     | `button?: "left" \| "middle" \| "right"`                                                | `boolean`              |
| mouseDrag      | `startX: number, startY: number, endX: number, endY: number, speed?: number`            | `boolean`              |
| typeString     | `stringToType: string, delay?: number`                                                  | `boolean`              |
| KeyListener.on | `event: "keyDown"`,<br>`callback: (data: { keyCode: number; keyName: string }) => void` | `this` (EventListener) |
| KeyListener.on | `event: "keyUp"`,<br>`callback: (data: { keyCode: number; keyName: string }) => void`   | `this` (EventListener) |

## Examples

Here are some examples of using the package:

```javascript
import {
  keyDownHandler,
  keyUpHandler,
  getWindowData,
  captureWindow,
  mouseMove,
  mouseClick,
  mouseDrag,
  typeString,
  KeyListener,
} from "node-native-win-utils";

// Register key event handlers

keyDownHandler((keyCode) => {
  console.log("Key down:", keyCode);
});

// Key down: 123

keyUpHandler((keyCode) => {
  console.log("Key up:", keyCode);
});

// Key up: 123

// Retrieve window data

const windowData = getWindowData("My Window");

console.log("Window data:", windowData);

// Window data: { width: 1024, height: 768, x: 100, y: 100 }

// Capture window screenshot

captureWindow("My Window", "output.png");

// Output: output.png with a screenshot of the window

// Move the mouse

mouseMove(100, 200);

// Perform mouse click

mouseClick(); // Left mouse click
mouseClick("right"); // Right mouse click

// Simulate mouse drag

mouseDrag(100, 200, 300, 400);
mouseDrag(100, 200, 300, 400, 100); // Drag with speed 100

// Simulate typing

typeString("Hello, world!");
typeString("Hello, world!", 100); // Type with a delay of 100ms between characters

// Use KeyListener class

const listener = new KeyListener();

listener.on("keyDown", (data) => {
  console.log("Key down:", data.keyCode, data.keyName);
});

// Key down: 8 Backspace

listener.on("keyUp", (data) => {
  console.log("Key up:", data.keyCode, data.keyName);
});

// Key up: 8 Backspace
```

P.S.: As my knowledge of C++ is just brief, most of the C++ code is written using GPT-3.5 and GPT-4 or found on Google with some minor edits by myself.

[license-src]: https://img.shields.io/github/license/nuxt-modules/icon.svg?style=for-the-badge&colorA=18181B&colorB=28CF8D
[license-href]: https://github.com/RynerNO/node-native-win-utils/blob/main/LICENSE
