const {
  getWindowData,
  takeWindowScreenshot,
} = require("./build/Release/window-data");
const path = require("path");
// Call the getWindowData function with the window name
takeWindowScreenshot(
  "@cellaro - Discord",
  path.join(__dirname, "test", "test.png")
);
console.log(getWindowData("EVE - Ryner- Lute"));
