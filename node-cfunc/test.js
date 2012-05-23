var a = require('./build/Release/cfunc');
var ret = a.cfunc(new Array(0xb8,0x05,0x00,0x00,0x00,0xc3));
console.log("Return:" + ret);

