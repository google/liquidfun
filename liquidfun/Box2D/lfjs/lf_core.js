// Note: For maximum-speed code, see "Optimizing Code" on the Emscripten wiki, https://github.com/kripken/emscripten/wiki/Optimizing-Code
// Note: Some Emscripten settings may limit the speed of the generated code.
// The Module object: Our interface to the outside world. We import
// and export values on it, and do the work to get that through
// closure compiler if necessary. There are various ways Module can be used:
// 1. Not defined. We create it here
// 2. A function parameter, function(Module) { ..generated code.. }
// 3. pre-run appended it, var Module = {}; ..generated code..
// 4. External script tag defines var Module.
// We need to do an eval in order to handle the closure compiler
// case, where this code here is minified but Module was defined
// elsewhere (e.g. case 4 above). We also need to check if Module
// already exists (e.g. case 3 above).
// Note that if you want to run closure, and also to use Module
// after the generated code, you will need to define   var Module = {};
// before the code. Then that object will be used in the code, and you
// can continue to use Module afterwards as well.
var Module;
if (!Module) Module = eval('(function() { try { return Module || {} } catch(e) { return {} } })()');

// Sometimes an existing Module object exists with properties
// meant to overwrite the default module functionality. Here
// we collect those properties and reapply _after_ we configure
// the current environment's defaults to avoid having to be so
// defensive during initialization.
var moduleOverrides = {};
for (var key in Module) {
  if (Module.hasOwnProperty(key)) {
    moduleOverrides[key] = Module[key];
  }
}

// The environment setup code below is customized to use Module.
// *** Environment setup code ***
var ENVIRONMENT_IS_NODE = typeof process === 'object' && typeof require === 'function';
var ENVIRONMENT_IS_WEB = typeof window === 'object';
var ENVIRONMENT_IS_WORKER = typeof importScripts === 'function';
var ENVIRONMENT_IS_SHELL = !ENVIRONMENT_IS_WEB && !ENVIRONMENT_IS_NODE && !ENVIRONMENT_IS_WORKER;

if (ENVIRONMENT_IS_NODE) {
  // Expose functionality in the same simple way that the shells work
  // Note that we pollute the global namespace here, otherwise we break in node
  if (!Module['print']) Module['print'] = function print(x) {
    process['stdout'].write(x + '\n');
  };
  if (!Module['printErr']) Module['printErr'] = function printErr(x) {
    process['stderr'].write(x + '\n');
  };

  var nodeFS = require('fs');
  var nodePath = require('path');

  Module['read'] = function read(filename, binary) {
    filename = nodePath['normalize'](filename);
    var ret = nodeFS['readFileSync'](filename);
    // The path is absolute if the normalized version is the same as the resolved.
    if (!ret && filename != nodePath['resolve'](filename)) {
      filename = path.join(__dirname, '..', 'src', filename);
      ret = nodeFS['readFileSync'](filename);
    }
    if (ret && !binary) ret = ret.toString();
    return ret;
  };

  Module['readBinary'] = function readBinary(filename) { return Module['read'](filename, true) };

  Module['load'] = function load(f) {
    globalEval(read(f));
  };

  Module['arguments'] = process['argv'].slice(2);

  module['exports'] = Module;
}
else if (ENVIRONMENT_IS_SHELL) {
  if (!Module['print']) Module['print'] = print;
  if (typeof printErr != 'undefined') Module['printErr'] = printErr; // not present in v8 or older sm

  if (typeof read != 'undefined') {
    Module['read'] = read;
  } else {
    Module['read'] = function read() { throw 'no read() available (jsc?)' };
  }

  Module['readBinary'] = function readBinary(f) {
    return read(f, 'binary');
  };

  if (typeof scriptArgs != 'undefined') {
    Module['arguments'] = scriptArgs;
  } else if (typeof arguments != 'undefined') {
    Module['arguments'] = arguments;
  }

  this['Module'] = Module;

  eval("if (typeof gc === 'function' && gc.toString().indexOf('[native code]') > 0) var gc = undefined"); // wipe out the SpiderMonkey shell 'gc' function, which can confuse closure (uses it as a minified name, and it is then initted to a non-falsey value unexpectedly)
}
else if (ENVIRONMENT_IS_WEB || ENVIRONMENT_IS_WORKER) {
  Module['read'] = function read(url) {
    var xhr = new XMLHttpRequest();
    xhr.open('GET', url, false);
    xhr.send(null);
    return xhr.responseText;
  };

  if (typeof arguments != 'undefined') {
    Module['arguments'] = arguments;
  }

  if (typeof console !== 'undefined') {
    if (!Module['print']) Module['print'] = function print(x) {
      console.log(x);
    };
    if (!Module['printErr']) Module['printErr'] = function printErr(x) {
      console.log(x);
    };
  } else {
    // Probably a worker, and without console.log. We can do very little here...
    var TRY_USE_DUMP = false;
    if (!Module['print']) Module['print'] = (TRY_USE_DUMP && (typeof(dump) !== "undefined") ? (function(x) {
      dump(x);
    }) : (function(x) {
      // self.postMessage(x); // enable this if you want stdout to be sent as messages
    }));
  }

  if (ENVIRONMENT_IS_WEB) {
    this['Module'] = Module;
  } else {
    Module['load'] = importScripts;
  }
}
else {
  // Unreachable because SHELL is dependant on the others
  throw 'Unknown runtime environment. Where are we?';
}

function globalEval(x) {
  eval.call(null, x);
}
if (!Module['load'] == 'undefined' && Module['read']) {
  Module['load'] = function load(f) {
    globalEval(Module['read'](f));
  };
}
if (!Module['print']) {
  Module['print'] = function(){};
}
if (!Module['printErr']) {
  Module['printErr'] = Module['print'];
}
if (!Module['arguments']) {
  Module['arguments'] = [];
}
// *** Environment setup code ***

// Closure helpers
Module.print = Module['print'];
Module.printErr = Module['printErr'];

// Callbacks
Module['preRun'] = [];
Module['postRun'] = [];

// Merge back in the overrides
for (var key in moduleOverrides) {
  if (moduleOverrides.hasOwnProperty(key)) {
    Module[key] = moduleOverrides[key];
  }
}



// === Auto-generated preamble library stuff ===

//========================================
// Runtime code shared with compiler
//========================================

var Runtime = {
  stackSave: function () {
    return STACKTOP;
  },
  stackRestore: function (stackTop) {
    STACKTOP = stackTop;
  },
  forceAlign: function (target, quantum) {
    quantum = quantum || 4;
    if (quantum == 1) return target;
    if (isNumber(target) && isNumber(quantum)) {
      return Math.ceil(target/quantum)*quantum;
    } else if (isNumber(quantum) && isPowerOfTwo(quantum)) {
      return '(((' +target + ')+' + (quantum-1) + ')&' + -quantum + ')';
    }
    return 'Math.ceil((' + target + ')/' + quantum + ')*' + quantum;
  },
  isNumberType: function (type) {
    return type in Runtime.INT_TYPES || type in Runtime.FLOAT_TYPES;
  },
  isPointerType: function isPointerType(type) {
  return type[type.length-1] == '*';
},
  isStructType: function isStructType(type) {
  if (isPointerType(type)) return false;
  if (isArrayType(type)) return true;
  if (/<?\{ ?[^}]* ?\}>?/.test(type)) return true; // { i32, i8 } etc. - anonymous struct types
  // See comment in isStructPointerType()
  return type[0] == '%';
},
  INT_TYPES: {"i1":0,"i8":0,"i16":0,"i32":0,"i64":0},
  FLOAT_TYPES: {"float":0,"double":0},
  or64: function (x, y) {
    var l = (x | 0) | (y | 0);
    var h = (Math.round(x / 4294967296) | Math.round(y / 4294967296)) * 4294967296;
    return l + h;
  },
  and64: function (x, y) {
    var l = (x | 0) & (y | 0);
    var h = (Math.round(x / 4294967296) & Math.round(y / 4294967296)) * 4294967296;
    return l + h;
  },
  xor64: function (x, y) {
    var l = (x | 0) ^ (y | 0);
    var h = (Math.round(x / 4294967296) ^ Math.round(y / 4294967296)) * 4294967296;
    return l + h;
  },
  getNativeTypeSize: function (type) {
    switch (type) {
      case 'i1': case 'i8': return 1;
      case 'i16': return 2;
      case 'i32': return 4;
      case 'i64': return 8;
      case 'float': return 4;
      case 'double': return 8;
      default: {
        if (type[type.length-1] === '*') {
          return Runtime.QUANTUM_SIZE; // A pointer
        } else if (type[0] === 'i') {
          var bits = parseInt(type.substr(1));
          assert(bits % 8 === 0);
          return bits/8;
        } else {
          return 0;
        }
      }
    }
  },
  getNativeFieldSize: function (type) {
    return Math.max(Runtime.getNativeTypeSize(type), Runtime.QUANTUM_SIZE);
  },
  dedup: function dedup(items, ident) {
  var seen = {};
  if (ident) {
    return items.filter(function(item) {
      if (seen[item[ident]]) return false;
      seen[item[ident]] = true;
      return true;
    });
  } else {
    return items.filter(function(item) {
      if (seen[item]) return false;
      seen[item] = true;
      return true;
    });
  }
},
  set: function set() {
  var args = typeof arguments[0] === 'object' ? arguments[0] : arguments;
  var ret = {};
  for (var i = 0; i < args.length; i++) {
    ret[args[i]] = 0;
  }
  return ret;
},
  STACK_ALIGN: 8,
  getAlignSize: function (type, size, vararg) {
    // we align i64s and doubles on 64-bit boundaries, unlike x86
    if (vararg) return 8;
    if (!vararg && (type == 'i64' || type == 'double')) return 8;
    if (!type) return Math.min(size, 8); // align structures internally to 64 bits
    return Math.min(size || (type ? Runtime.getNativeFieldSize(type) : 0), Runtime.QUANTUM_SIZE);
  },
  calculateStructAlignment: function calculateStructAlignment(type) {
    type.flatSize = 0;
    type.alignSize = 0;
    var diffs = [];
    var prev = -1;
    var index = 0;
    type.flatIndexes = type.fields.map(function(field) {
      index++;
      var size, alignSize;
      if (Runtime.isNumberType(field) || Runtime.isPointerType(field)) {
        size = Runtime.getNativeTypeSize(field); // pack char; char; in structs, also char[X]s.
        alignSize = Runtime.getAlignSize(field, size);
      } else if (Runtime.isStructType(field)) {
        if (field[1] === '0') {
          // this is [0 x something]. When inside another structure like here, it must be at the end,
          // and it adds no size
          // XXX this happens in java-nbody for example... assert(index === type.fields.length, 'zero-length in the middle!');
          size = 0;
          if (Types.types[field]) {
            alignSize = Runtime.getAlignSize(null, Types.types[field].alignSize);
          } else {
            alignSize = type.alignSize || QUANTUM_SIZE;
          }
        } else {
          size = Types.types[field].flatSize;
          alignSize = Runtime.getAlignSize(null, Types.types[field].alignSize);
        }
      } else if (field[0] == 'b') {
        // bN, large number field, like a [N x i8]
        size = field.substr(1)|0;
        alignSize = 1;
      } else if (field[0] === '<') {
        // vector type
        size = alignSize = Types.types[field].flatSize; // fully aligned
      } else if (field[0] === 'i') {
        // illegal integer field, that could not be legalized because it is an internal structure field
        // it is ok to have such fields, if we just use them as markers of field size and nothing more complex
        size = alignSize = parseInt(field.substr(1))/8;
        assert(size % 1 === 0, 'cannot handle non-byte-size field ' + field);
      } else {
        assert(false, 'invalid type for calculateStructAlignment');
      }
      if (type.packed) alignSize = 1;
      type.alignSize = Math.max(type.alignSize, alignSize);
      var curr = Runtime.alignMemory(type.flatSize, alignSize); // if necessary, place this on aligned memory
      type.flatSize = curr + size;
      if (prev >= 0) {
        diffs.push(curr-prev);
      }
      prev = curr;
      return curr;
    });
    if (type.name_ && type.name_[0] === '[') {
      // arrays have 2 elements, so we get the proper difference. then we scale here. that way we avoid
      // allocating a potentially huge array for [999999 x i8] etc.
      type.flatSize = parseInt(type.name_.substr(1))*type.flatSize/2;
    }
    type.flatSize = Runtime.alignMemory(type.flatSize, type.alignSize);
    if (diffs.length == 0) {
      type.flatFactor = type.flatSize;
    } else if (Runtime.dedup(diffs).length == 1) {
      type.flatFactor = diffs[0];
    }
    type.needsFlattening = (type.flatFactor != 1);
    return type.flatIndexes;
  },
  generateStructInfo: function (struct, typeName, offset) {
    var type, alignment;
    if (typeName) {
      offset = offset || 0;
      type = (typeof Types === 'undefined' ? Runtime.typeInfo : Types.types)[typeName];
      if (!type) return null;
      if (type.fields.length != struct.length) {
        printErr('Number of named fields must match the type for ' + typeName + ': possibly duplicate struct names. Cannot return structInfo');
        return null;
      }
      alignment = type.flatIndexes;
    } else {
      var type = { fields: struct.map(function(item) { return item[0] }) };
      alignment = Runtime.calculateStructAlignment(type);
    }
    var ret = {
      __size__: type.flatSize
    };
    if (typeName) {
      struct.forEach(function(item, i) {
        if (typeof item === 'string') {
          ret[item] = alignment[i] + offset;
        } else {
          // embedded struct
          var key;
          for (var k in item) key = k;
          ret[key] = Runtime.generateStructInfo(item[key], type.fields[i], alignment[i]);
        }
      });
    } else {
      struct.forEach(function(item, i) {
        ret[item[1]] = alignment[i];
      });
    }
    return ret;
  },
  dynCall: function (sig, ptr, args) {
    if (args && args.length) {
      if (!args.splice) args = Array.prototype.slice.call(args);
      args.splice(0, 0, ptr);
      return Module['dynCall_' + sig].apply(null, args);
    } else {
      return Module['dynCall_' + sig].call(null, ptr);
    }
  },
  functionPointers: [],
  addFunction: function (func) {
    for (var i = 0; i < Runtime.functionPointers.length; i++) {
      if (!Runtime.functionPointers[i]) {
        Runtime.functionPointers[i] = func;
        return 2*(1 + i);
      }
    }
    throw 'Finished up all reserved function pointers. Use a higher value for RESERVED_FUNCTION_POINTERS.';
  },
  removeFunction: function (index) {
    Runtime.functionPointers[(index-2)/2] = null;
  },
  getAsmConst: function (code, numArgs) {
    // code is a constant string on the heap, so we can cache these
    if (!Runtime.asmConstCache) Runtime.asmConstCache = {};
    var func = Runtime.asmConstCache[code];
    if (func) return func;
    var args = [];
    for (var i = 0; i < numArgs; i++) {
      args.push(String.fromCharCode(36) + i); // $0, $1 etc
    }
    code = Pointer_stringify(code);
    if (code[0] === '"') {
      // tolerate EM_ASM("..code..") even though EM_ASM(..code..) is correct
      if (code.indexOf('"', 1) === code.length-1) {
        code = code.substr(1, code.length-2);
      } else {
        // something invalid happened, e.g. EM_ASM("..code($0)..", input)
        abort('invalid EM_ASM input |' + code + '|. Please use EM_ASM(..code..) (no quotes) or EM_ASM({ ..code($0).. }, input) (to input values)');
      }
    }
    return Runtime.asmConstCache[code] = eval('(function(' + args.join(',') + '){ ' + code + ' })'); // new Function does not allow upvars in node
  },
  warnOnce: function (text) {
    if (!Runtime.warnOnce.shown) Runtime.warnOnce.shown = {};
    if (!Runtime.warnOnce.shown[text]) {
      Runtime.warnOnce.shown[text] = 1;
      Module.printErr(text);
    }
  },
  funcWrappers: {},
  getFuncWrapper: function (func, sig) {
    assert(sig);
    if (!Runtime.funcWrappers[func]) {
      Runtime.funcWrappers[func] = function dynCall_wrapper() {
        return Runtime.dynCall(sig, func, arguments);
      };
    }
    return Runtime.funcWrappers[func];
  },
  UTF8Processor: function () {
    var buffer = [];
    var needed = 0;
    this.processCChar = function (code) {
      code = code & 0xFF;

      if (buffer.length == 0) {
        if ((code & 0x80) == 0x00) {        // 0xxxxxxx
          return String.fromCharCode(code);
        }
        buffer.push(code);
        if ((code & 0xE0) == 0xC0) {        // 110xxxxx
          needed = 1;
        } else if ((code & 0xF0) == 0xE0) { // 1110xxxx
          needed = 2;
        } else {                            // 11110xxx
          needed = 3;
        }
        return '';
      }

      if (needed) {
        buffer.push(code);
        needed--;
        if (needed > 0) return '';
      }

      var c1 = buffer[0];
      var c2 = buffer[1];
      var c3 = buffer[2];
      var c4 = buffer[3];
      var ret;
      if (buffer.length == 2) {
        ret = String.fromCharCode(((c1 & 0x1F) << 6)  | (c2 & 0x3F));
      } else if (buffer.length == 3) {
        ret = String.fromCharCode(((c1 & 0x0F) << 12) | ((c2 & 0x3F) << 6)  | (c3 & 0x3F));
      } else {
        // http://mathiasbynens.be/notes/javascript-encoding#surrogate-formulae
        var codePoint = ((c1 & 0x07) << 18) | ((c2 & 0x3F) << 12) |
                        ((c3 & 0x3F) << 6)  | (c4 & 0x3F);
        ret = String.fromCharCode(
          Math.floor((codePoint - 0x10000) / 0x400) + 0xD800,
          (codePoint - 0x10000) % 0x400 + 0xDC00);
      }
      buffer.length = 0;
      return ret;
    }
    this.processJSString = function processJSString(string) {
      string = unescape(encodeURIComponent(string));
      var ret = [];
      for (var i = 0; i < string.length; i++) {
        ret.push(string.charCodeAt(i));
      }
      return ret;
    }
  },
  getCompilerSetting: function (name) {
    throw 'You must build with -s RETAIN_COMPILER_SETTINGS=1 for Runtime.getCompilerSetting or emscripten_get_compiler_setting to work';
  },
  stackAlloc: function (size) { var ret = STACKTOP;STACKTOP = (STACKTOP + size)|0;STACKTOP = (((STACKTOP)+7)&-8); return ret; },
  staticAlloc: function (size) { var ret = STATICTOP;STATICTOP = (STATICTOP + size)|0;STATICTOP = (((STATICTOP)+7)&-8); return ret; },
  dynamicAlloc: function (size) { var ret = DYNAMICTOP;DYNAMICTOP = (DYNAMICTOP + size)|0;DYNAMICTOP = (((DYNAMICTOP)+7)&-8); if (DYNAMICTOP >= TOTAL_MEMORY) enlargeMemory();; return ret; },
  alignMemory: function (size,quantum) { var ret = size = Math.ceil((size)/(quantum ? quantum : 8))*(quantum ? quantum : 8); return ret; },
  makeBigInt: function (low,high,unsigned) { var ret = (unsigned ? ((+((low>>>0)))+((+((high>>>0)))*(+4294967296))) : ((+((low>>>0)))+((+((high|0)))*(+4294967296)))); return ret; },
  GLOBAL_BASE: 8,
  QUANTUM_SIZE: 4,
  __dummy__: 0
}


Module['Runtime'] = Runtime;









//========================================
// Runtime essentials
//========================================

var __THREW__ = 0; // Used in checking for thrown exceptions.

var ABORT = false; // whether we are quitting the application. no code should run after this. set in exit() and abort()
var EXITSTATUS = 0;

var undef = 0;
// tempInt is used for 32-bit signed values or smaller. tempBigInt is used
// for 32-bit unsigned values or more than 32 bits. TODO: audit all uses of tempInt
var tempValue, tempInt, tempBigInt, tempInt2, tempBigInt2, tempPair, tempBigIntI, tempBigIntR, tempBigIntS, tempBigIntP, tempBigIntD, tempDouble, tempFloat;
var tempI64, tempI64b;
var tempRet0, tempRet1, tempRet2, tempRet3, tempRet4, tempRet5, tempRet6, tempRet7, tempRet8, tempRet9;

function assert(condition, text) {
  if (!condition) {
    abort('Assertion failed: ' + text);
  }
}

var globalScope = this;

// C calling interface. A convenient way to call C functions (in C files, or
// defined with extern "C").
//
// Note: LLVM optimizations can inline and remove functions, after which you will not be
//       able to call them. Closure can also do so. To avoid that, add your function to
//       the exports using something like
//
//         -s EXPORTED_FUNCTIONS='["_main", "_myfunc"]'
//
// @param ident      The name of the C function (note that C++ functions will be name-mangled - use extern "C")
// @param returnType The return type of the function, one of the JS types 'number', 'string' or 'array' (use 'number' for any C pointer, and
//                   'array' for JavaScript arrays and typed arrays; note that arrays are 8-bit).
// @param argTypes   An array of the types of arguments for the function (if there are no arguments, this can be ommitted). Types are as in returnType,
//                   except that 'array' is not possible (there is no way for us to know the length of the array)
// @param args       An array of the arguments to the function, as native JS values (as in returnType)
//                   Note that string arguments will be stored on the stack (the JS string will become a C string on the stack).
// @return           The return value, as a native JS value (as in returnType)
function ccall(ident, returnType, argTypes, args) {
  return ccallFunc(getCFunc(ident), returnType, argTypes, args);
}
Module["ccall"] = ccall;

// Returns the C function with a specified identifier (for C++, you need to do manual name mangling)
function getCFunc(ident) {
  try {
    var func = Module['_' + ident]; // closure exported function
    if (!func) func = eval('_' + ident); // explicit lookup
  } catch(e) {
  }
  assert(func, 'Cannot call unknown function ' + ident + ' (perhaps LLVM optimizations or closure removed it?)');
  return func;
}

// Internal function that does a C call using a function, not an identifier
function ccallFunc(func, returnType, argTypes, args) {
  var stack = 0;
  function toC(value, type) {
    if (type == 'string') {
      if (value === null || value === undefined || value === 0) return 0; // null string
      value = intArrayFromString(value);
      type = 'array';
    }
    if (type == 'array') {
      if (!stack) stack = Runtime.stackSave();
      var ret = Runtime.stackAlloc(value.length);
      writeArrayToMemory(value, ret);
      return ret;
    }
    return value;
  }
  function fromC(value, type) {
    if (type == 'string') {
      return Pointer_stringify(value);
    }
    assert(type != 'array');
    return value;
  }
  var i = 0;
  var cArgs = args ? args.map(function(arg) {
    return toC(arg, argTypes[i++]);
  }) : [];
  var ret = fromC(func.apply(null, cArgs), returnType);
  if (stack) Runtime.stackRestore(stack);
  return ret;
}

// Returns a native JS wrapper for a C function. This is similar to ccall, but
// returns a function you can call repeatedly in a normal way. For example:
//
//   var my_function = cwrap('my_c_function', 'number', ['number', 'number']);
//   alert(my_function(5, 22));
//   alert(my_function(99, 12));
//
function cwrap(ident, returnType, argTypes) {
  var func = getCFunc(ident);
  return function() {
    return ccallFunc(func, returnType, argTypes, Array.prototype.slice.call(arguments));
  }
}
Module["cwrap"] = cwrap;

// Sets a value in memory in a dynamic way at run-time. Uses the
// type data. This is the same as makeSetValue, except that
// makeSetValue is done at compile-time and generates the needed
// code then, whereas this function picks the right code at
// run-time.
// Note that setValue and getValue only do *aligned* writes and reads!
// Note that ccall uses JS types as for defining types, while setValue and
// getValue need LLVM types ('i8', 'i32') - this is a lower-level operation
function setValue(ptr, value, type, noSafe) {
  type = type || 'i8';
  if (type.charAt(type.length-1) === '*') type = 'i32'; // pointers are 32-bit
    switch(type) {
      case 'i1': HEAP8[(ptr)]=value; break;
      case 'i8': HEAP8[(ptr)]=value; break;
      case 'i16': HEAP16[((ptr)>>1)]=value; break;
      case 'i32': HEAP32[((ptr)>>2)]=value; break;
      case 'i64': (tempI64 = [value>>>0,(tempDouble=value,(+(Math_abs(tempDouble))) >= (+1) ? (tempDouble > (+0) ? ((Math_min((+(Math_floor((tempDouble)/(+4294967296)))), (+4294967295)))|0)>>>0 : (~~((+(Math_ceil((tempDouble - +(((~~(tempDouble)))>>>0))/(+4294967296))))))>>>0) : 0)],HEAP32[((ptr)>>2)]=tempI64[0],HEAP32[(((ptr)+(4))>>2)]=tempI64[1]); break;
      case 'float': HEAPF32[((ptr)>>2)]=value; break;
      case 'double': HEAPF64[((ptr)>>3)]=value; break;
      default: abort('invalid type for setValue: ' + type);
    }
}
Module['setValue'] = setValue;

// Parallel to setValue.
function getValue(ptr, type, noSafe) {
  type = type || 'i8';
  if (type.charAt(type.length-1) === '*') type = 'i32'; // pointers are 32-bit
    switch(type) {
      case 'i1': return HEAP8[(ptr)];
      case 'i8': return HEAP8[(ptr)];
      case 'i16': return HEAP16[((ptr)>>1)];
      case 'i32': return HEAP32[((ptr)>>2)];
      case 'i64': return HEAP32[((ptr)>>2)];
      case 'float': return HEAPF32[((ptr)>>2)];
      case 'double': return HEAPF64[((ptr)>>3)];
      default: abort('invalid type for setValue: ' + type);
    }
  return null;
}
Module['getValue'] = getValue;

var ALLOC_NORMAL = 0; // Tries to use _malloc()
var ALLOC_STACK = 1; // Lives for the duration of the current function call
var ALLOC_STATIC = 2; // Cannot be freed
var ALLOC_DYNAMIC = 3; // Cannot be freed except through sbrk
var ALLOC_NONE = 4; // Do not allocate
Module['ALLOC_NORMAL'] = ALLOC_NORMAL;
Module['ALLOC_STACK'] = ALLOC_STACK;
Module['ALLOC_STATIC'] = ALLOC_STATIC;
Module['ALLOC_DYNAMIC'] = ALLOC_DYNAMIC;
Module['ALLOC_NONE'] = ALLOC_NONE;

// allocate(): This is for internal use. You can use it yourself as well, but the interface
//             is a little tricky (see docs right below). The reason is that it is optimized
//             for multiple syntaxes to save space in generated code. So you should
//             normally not use allocate(), and instead allocate memory using _malloc(),
//             initialize it with setValue(), and so forth.
// @slab: An array of data, or a number. If a number, then the size of the block to allocate,
//        in *bytes* (note that this is sometimes confusing: the next parameter does not
//        affect this!)
// @types: Either an array of types, one for each byte (or 0 if no type at that position),
//         or a single type which is used for the entire block. This only matters if there
//         is initial data - if @slab is a number, then this does not matter at all and is
//         ignored.
// @allocator: How to allocate memory, see ALLOC_*
function allocate(slab, types, allocator, ptr) {
  var zeroinit, size;
  if (typeof slab === 'number') {
    zeroinit = true;
    size = slab;
  } else {
    zeroinit = false;
    size = slab.length;
  }

  var singleType = typeof types === 'string' ? types : null;

  var ret;
  if (allocator == ALLOC_NONE) {
    ret = ptr;
  } else {
    ret = [_malloc, Runtime.stackAlloc, Runtime.staticAlloc, Runtime.dynamicAlloc][allocator === undefined ? ALLOC_STATIC : allocator](Math.max(size, singleType ? 1 : types.length));
  }

  if (zeroinit) {
    var ptr = ret, stop;
    assert((ret & 3) == 0);
    stop = ret + (size & ~3);
    for (; ptr < stop; ptr += 4) {
      HEAP32[((ptr)>>2)]=0;
    }
    stop = ret + size;
    while (ptr < stop) {
      HEAP8[((ptr++)|0)]=0;
    }
    return ret;
  }

  if (singleType === 'i8') {
    if (slab.subarray || slab.slice) {
      HEAPU8.set(slab, ret);
    } else {
      HEAPU8.set(new Uint8Array(slab), ret);
    }
    return ret;
  }

  var i = 0, type, typeSize, previousType;
  while (i < size) {
    var curr = slab[i];

    if (typeof curr === 'function') {
      curr = Runtime.getFunctionIndex(curr);
    }

    type = singleType || types[i];
    if (type === 0) {
      i++;
      continue;
    }

    if (type == 'i64') type = 'i32'; // special case: we have one i32 here, and one i32 later

    setValue(ret+i, curr, type);

    // no need to look up size unless type changes, so cache it
    if (previousType !== type) {
      typeSize = Runtime.getNativeTypeSize(type);
      previousType = type;
    }
    i += typeSize;
  }

  return ret;
}
Module['allocate'] = allocate;

function Pointer_stringify(ptr, /* optional */ length) {
  // TODO: use TextDecoder
  // Find the length, and check for UTF while doing so
  var hasUtf = false;
  var t;
  var i = 0;
  while (1) {
    t = HEAPU8[(((ptr)+(i))|0)];
    if (t >= 128) hasUtf = true;
    else if (t == 0 && !length) break;
    i++;
    if (length && i == length) break;
  }
  if (!length) length = i;

  var ret = '';

  if (!hasUtf) {
    var MAX_CHUNK = 1024; // split up into chunks, because .apply on a huge string can overflow the stack
    var curr;
    while (length > 0) {
      curr = String.fromCharCode.apply(String, HEAPU8.subarray(ptr, ptr + Math.min(length, MAX_CHUNK)));
      ret = ret ? ret + curr : curr;
      ptr += MAX_CHUNK;
      length -= MAX_CHUNK;
    }
    return ret;
  }

  var utf8 = new Runtime.UTF8Processor();
  for (i = 0; i < length; i++) {
    t = HEAPU8[(((ptr)+(i))|0)];
    ret += utf8.processCChar(t);
  }
  return ret;
}
Module['Pointer_stringify'] = Pointer_stringify;

// Given a pointer 'ptr' to a null-terminated UTF16LE-encoded string in the emscripten HEAP, returns
// a copy of that string as a Javascript String object.
function UTF16ToString(ptr) {
  var i = 0;

  var str = '';
  while (1) {
    var codeUnit = HEAP16[(((ptr)+(i*2))>>1)];
    if (codeUnit == 0)
      return str;
    ++i;
    // fromCharCode constructs a character from a UTF-16 code unit, so we can pass the UTF16 string right through.
    str += String.fromCharCode(codeUnit);
  }
}
Module['UTF16ToString'] = UTF16ToString;

// Copies the given Javascript String object 'str' to the emscripten HEAP at address 'outPtr',
// null-terminated and encoded in UTF16LE form. The copy will require at most (str.length*2+1)*2 bytes of space in the HEAP.
function stringToUTF16(str, outPtr) {
  for(var i = 0; i < str.length; ++i) {
    // charCodeAt returns a UTF-16 encoded code unit, so it can be directly written to the HEAP.
    var codeUnit = str.charCodeAt(i); // possibly a lead surrogate
    HEAP16[(((outPtr)+(i*2))>>1)]=codeUnit;
  }
  // Null-terminate the pointer to the HEAP.
  HEAP16[(((outPtr)+(str.length*2))>>1)]=0;
}
Module['stringToUTF16'] = stringToUTF16;

// Given a pointer 'ptr' to a null-terminated UTF32LE-encoded string in the emscripten HEAP, returns
// a copy of that string as a Javascript String object.
function UTF32ToString(ptr) {
  var i = 0;

  var str = '';
  while (1) {
    var utf32 = HEAP32[(((ptr)+(i*4))>>2)];
    if (utf32 == 0)
      return str;
    ++i;
    // Gotcha: fromCharCode constructs a character from a UTF-16 encoded code (pair), not from a Unicode code point! So encode the code point to UTF-16 for constructing.
    if (utf32 >= 0x10000) {
      var ch = utf32 - 0x10000;
      str += String.fromCharCode(0xD800 | (ch >> 10), 0xDC00 | (ch & 0x3FF));
    } else {
      str += String.fromCharCode(utf32);
    }
  }
}
Module['UTF32ToString'] = UTF32ToString;

// Copies the given Javascript String object 'str' to the emscripten HEAP at address 'outPtr',
// null-terminated and encoded in UTF32LE form. The copy will require at most (str.length+1)*4 bytes of space in the HEAP,
// but can use less, since str.length does not return the number of characters in the string, but the number of UTF-16 code units in the string.
function stringToUTF32(str, outPtr) {
  var iChar = 0;
  for(var iCodeUnit = 0; iCodeUnit < str.length; ++iCodeUnit) {
    // Gotcha: charCodeAt returns a 16-bit word that is a UTF-16 encoded code unit, not a Unicode code point of the character! We must decode the string to UTF-32 to the heap.
    var codeUnit = str.charCodeAt(iCodeUnit); // possibly a lead surrogate
    if (codeUnit >= 0xD800 && codeUnit <= 0xDFFF) {
      var trailSurrogate = str.charCodeAt(++iCodeUnit);
      codeUnit = 0x10000 + ((codeUnit & 0x3FF) << 10) | (trailSurrogate & 0x3FF);
    }
    HEAP32[(((outPtr)+(iChar*4))>>2)]=codeUnit;
    ++iChar;
  }
  // Null-terminate the pointer to the HEAP.
  HEAP32[(((outPtr)+(iChar*4))>>2)]=0;
}
Module['stringToUTF32'] = stringToUTF32;

function demangle(func) {
  var i = 3;
  // params, etc.
  var basicTypes = {
    'v': 'void',
    'b': 'bool',
    'c': 'char',
    's': 'short',
    'i': 'int',
    'l': 'long',
    'f': 'float',
    'd': 'double',
    'w': 'wchar_t',
    'a': 'signed char',
    'h': 'unsigned char',
    't': 'unsigned short',
    'j': 'unsigned int',
    'm': 'unsigned long',
    'x': 'long long',
    'y': 'unsigned long long',
    'z': '...'
  };
  var subs = [];
  var first = true;
  function dump(x) {
    //return;
    if (x) Module.print(x);
    Module.print(func);
    var pre = '';
    for (var a = 0; a < i; a++) pre += ' ';
    Module.print (pre + '^');
  }
  function parseNested() {
    i++;
    if (func[i] === 'K') i++; // ignore const
    var parts = [];
    while (func[i] !== 'E') {
      if (func[i] === 'S') { // substitution
        i++;
        var next = func.indexOf('_', i);
        var num = func.substring(i, next) || 0;
        parts.push(subs[num] || '?');
        i = next+1;
        continue;
      }
      if (func[i] === 'C') { // constructor
        parts.push(parts[parts.length-1]);
        i += 2;
        continue;
      }
      var size = parseInt(func.substr(i));
      var pre = size.toString().length;
      if (!size || !pre) { i--; break; } // counter i++ below us
      var curr = func.substr(i + pre, size);
      parts.push(curr);
      subs.push(curr);
      i += pre + size;
    }
    i++; // skip E
    return parts;
  }
  function parse(rawList, limit, allowVoid) { // main parser
    limit = limit || Infinity;
    var ret = '', list = [];
    function flushList() {
      return '(' + list.join(', ') + ')';
    }
    var name;
    if (func[i] === 'N') {
      // namespaced N-E
      name = parseNested().join('::');
      limit--;
      if (limit === 0) return rawList ? [name] : name;
    } else {
      // not namespaced
      if (func[i] === 'K' || (first && func[i] === 'L')) i++; // ignore const and first 'L'
      var size = parseInt(func.substr(i));
      if (size) {
        var pre = size.toString().length;
        name = func.substr(i + pre, size);
        i += pre + size;
      }
    }
    first = false;
    if (func[i] === 'I') {
      i++;
      var iList = parse(true);
      var iRet = parse(true, 1, true);
      ret += iRet[0] + ' ' + name + '<' + iList.join(', ') + '>';
    } else {
      ret = name;
    }
    paramLoop: while (i < func.length && limit-- > 0) {
      //dump('paramLoop');
      var c = func[i++];
      if (c in basicTypes) {
        list.push(basicTypes[c]);
      } else {
        switch (c) {
          case 'P': list.push(parse(true, 1, true)[0] + '*'); break; // pointer
          case 'R': list.push(parse(true, 1, true)[0] + '&'); break; // reference
          case 'L': { // literal
            i++; // skip basic type
            var end = func.indexOf('E', i);
            var size = end - i;
            list.push(func.substr(i, size));
            i += size + 2; // size + 'EE'
            break;
          }
          case 'A': { // array
            var size = parseInt(func.substr(i));
            i += size.toString().length;
            if (func[i] !== '_') throw '?';
            i++; // skip _
            list.push(parse(true, 1, true)[0] + ' [' + size + ']');
            break;
          }
          case 'E': break paramLoop;
          default: ret += '?' + c; break paramLoop;
        }
      }
    }
    if (!allowVoid && list.length === 1 && list[0] === 'void') list = []; // avoid (void)
    return rawList ? list : ret + flushList();
  }
  try {
    // Special-case the entry point, since its name differs from other name mangling.
    if (func == 'Object._main' || func == '_main') {
      return 'main()';
    }
    if (typeof func === 'number') func = Pointer_stringify(func);
    if (func[0] !== '_') return func;
    if (func[1] !== '_') return func; // C function
    if (func[2] !== 'Z') return func;
    switch (func[3]) {
      case 'n': return 'operator new()';
      case 'd': return 'operator delete()';
    }
    return parse();
  } catch(e) {
    return func;
  }
}

function demangleAll(text) {
  return text.replace(/__Z[\w\d_]+/g, function(x) { var y = demangle(x); return x === y ? x : (x + ' [' + y + ']') });
}

function stackTrace() {
  var stack = new Error().stack;
  return stack ? demangleAll(stack) : '(no stack trace available)'; // Stack trace is not available at least on IE10 and Safari 6.
}

// Memory management

var PAGE_SIZE = 4096;
function alignMemoryPage(x) {
  return (x+4095)&-4096;
}

var HEAP;
var HEAP8, HEAPU8, HEAP16, HEAPU16, HEAP32, HEAPU32, HEAPF32, HEAPF64;

var STATIC_BASE = 0, STATICTOP = 0, staticSealed = false; // static area
var STACK_BASE = 0, STACKTOP = 0, STACK_MAX = 0; // stack area
var DYNAMIC_BASE = 0, DYNAMICTOP = 0; // dynamic area handled by sbrk

function enlargeMemory() {
  abort('Cannot enlarge memory arrays. Either (1) compile with -s TOTAL_MEMORY=X with X higher than the current value ' + TOTAL_MEMORY + ', (2) compile with ALLOW_MEMORY_GROWTH which adjusts the size at runtime but prevents some optimizations, or (3) set Module.TOTAL_MEMORY before the program runs.');
}

var TOTAL_STACK = Module['TOTAL_STACK'] || 5242880;
var TOTAL_MEMORY = Module['TOTAL_MEMORY'] || 33554432;
var FAST_MEMORY = Module['FAST_MEMORY'] || 2097152;

var totalMemory = 4096;
while (totalMemory < TOTAL_MEMORY || totalMemory < 2*TOTAL_STACK) {
  if (totalMemory < 16*1024*1024) {
    totalMemory *= 2;
  } else {
    totalMemory += 16*1024*1024
  }
}
if (totalMemory !== TOTAL_MEMORY) {
  Module.printErr('increasing TOTAL_MEMORY to ' + totalMemory + ' to be more reasonable');
  TOTAL_MEMORY = totalMemory;
}

// Initialize the runtime's memory
// check for full engine support (use string 'subarray' to avoid closure compiler confusion)
assert(typeof Int32Array !== 'undefined' && typeof Float64Array !== 'undefined' && !!(new Int32Array(1)['subarray']) && !!(new Int32Array(1)['set']),
       'JS engine does not provide full typed array support');

var buffer = new ArrayBuffer(TOTAL_MEMORY);
HEAP8 = new Int8Array(buffer);
HEAP16 = new Int16Array(buffer);
HEAP32 = new Int32Array(buffer);
HEAPU8 = new Uint8Array(buffer);
HEAPU16 = new Uint16Array(buffer);
HEAPU32 = new Uint32Array(buffer);
HEAPF32 = new Float32Array(buffer);
HEAPF64 = new Float64Array(buffer);

// Endianness check (note: assumes compiler arch was little-endian)
HEAP32[0] = 255;
assert(HEAPU8[0] === 255 && HEAPU8[3] === 0, 'Typed arrays 2 must be run on a little-endian system');

Module['HEAP'] = HEAP;
Module['HEAP8'] = HEAP8;
Module['HEAP16'] = HEAP16;
Module['HEAP32'] = HEAP32;
Module['HEAPU8'] = HEAPU8;
Module['HEAPU16'] = HEAPU16;
Module['HEAPU32'] = HEAPU32;
Module['HEAPF32'] = HEAPF32;
Module['HEAPF64'] = HEAPF64;

function callRuntimeCallbacks(callbacks) {
  while(callbacks.length > 0) {
    var callback = callbacks.shift();
    if (typeof callback == 'function') {
      callback();
      continue;
    }
    var func = callback.func;
    if (typeof func === 'number') {
      if (callback.arg === undefined) {
        Runtime.dynCall('v', func);
      } else {
        Runtime.dynCall('vi', func, [callback.arg]);
      }
    } else {
      func(callback.arg === undefined ? null : callback.arg);
    }
  }
}

var __ATPRERUN__  = []; // functions called before the runtime is initialized
var __ATINIT__    = []; // functions called during startup
var __ATMAIN__    = []; // functions called when main() is to be run
var __ATEXIT__    = []; // functions called during shutdown
var __ATPOSTRUN__ = []; // functions called after the runtime has exited

var runtimeInitialized = false;

function preRun() {
  // compatibility - merge in anything from Module['preRun'] at this time
  if (Module['preRun']) {
    if (typeof Module['preRun'] == 'function') Module['preRun'] = [Module['preRun']];
    while (Module['preRun'].length) {
      addOnPreRun(Module['preRun'].shift());
    }
  }
  callRuntimeCallbacks(__ATPRERUN__);
}

function ensureInitRuntime() {
  if (runtimeInitialized) return;
  runtimeInitialized = true;
  callRuntimeCallbacks(__ATINIT__);
}

function preMain() {
  callRuntimeCallbacks(__ATMAIN__);
}

function exitRuntime() {
  callRuntimeCallbacks(__ATEXIT__);
}

function postRun() {
  // compatibility - merge in anything from Module['postRun'] at this time
  if (Module['postRun']) {
    if (typeof Module['postRun'] == 'function') Module['postRun'] = [Module['postRun']];
    while (Module['postRun'].length) {
      addOnPostRun(Module['postRun'].shift());
    }
  }
  callRuntimeCallbacks(__ATPOSTRUN__);
}

function addOnPreRun(cb) {
  __ATPRERUN__.unshift(cb);
}
Module['addOnPreRun'] = Module.addOnPreRun = addOnPreRun;

function addOnInit(cb) {
  __ATINIT__.unshift(cb);
}
Module['addOnInit'] = Module.addOnInit = addOnInit;

function addOnPreMain(cb) {
  __ATMAIN__.unshift(cb);
}
Module['addOnPreMain'] = Module.addOnPreMain = addOnPreMain;

function addOnExit(cb) {
  __ATEXIT__.unshift(cb);
}
Module['addOnExit'] = Module.addOnExit = addOnExit;

function addOnPostRun(cb) {
  __ATPOSTRUN__.unshift(cb);
}
Module['addOnPostRun'] = Module.addOnPostRun = addOnPostRun;

// Tools

// This processes a JS string into a C-line array of numbers, 0-terminated.
// For LLVM-originating strings, see parser.js:parseLLVMString function
function intArrayFromString(stringy, dontAddNull, length /* optional */) {
  var ret = (new Runtime.UTF8Processor()).processJSString(stringy);
  if (length) {
    ret.length = length;
  }
  if (!dontAddNull) {
    ret.push(0);
  }
  return ret;
}
Module['intArrayFromString'] = intArrayFromString;

function intArrayToString(array) {
  var ret = [];
  for (var i = 0; i < array.length; i++) {
    var chr = array[i];
    if (chr > 0xFF) {
      chr &= 0xFF;
    }
    ret.push(String.fromCharCode(chr));
  }
  return ret.join('');
}
Module['intArrayToString'] = intArrayToString;

// Write a Javascript array to somewhere in the heap
function writeStringToMemory(string, buffer, dontAddNull) {
  var array = intArrayFromString(string, dontAddNull);
  var i = 0;
  while (i < array.length) {
    var chr = array[i];
    HEAP8[(((buffer)+(i))|0)]=chr;
    i = i + 1;
  }
}
Module['writeStringToMemory'] = writeStringToMemory;

function writeArrayToMemory(array, buffer) {
  for (var i = 0; i < array.length; i++) {
    HEAP8[(((buffer)+(i))|0)]=array[i];
  }
}
Module['writeArrayToMemory'] = writeArrayToMemory;

function writeAsciiToMemory(str, buffer, dontAddNull) {
  for (var i = 0; i < str.length; i++) {
    HEAP8[(((buffer)+(i))|0)]=str.charCodeAt(i);
  }
  if (!dontAddNull) HEAP8[(((buffer)+(str.length))|0)]=0;
}
Module['writeAsciiToMemory'] = writeAsciiToMemory;

function unSign(value, bits, ignore) {
  if (value >= 0) {
    return value;
  }
  return bits <= 32 ? 2*Math.abs(1 << (bits-1)) + value // Need some trickery, since if bits == 32, we are right at the limit of the bits JS uses in bitshifts
                    : Math.pow(2, bits)         + value;
}
function reSign(value, bits, ignore) {
  if (value <= 0) {
    return value;
  }
  var half = bits <= 32 ? Math.abs(1 << (bits-1)) // abs is needed if bits == 32
                        : Math.pow(2, bits-1);
  if (value >= half && (bits <= 32 || value > half)) { // for huge values, we can hit the precision limit and always get true here. so don't do that
                                                       // but, in general there is no perfect solution here. With 64-bit ints, we get rounding and errors
                                                       // TODO: In i64 mode 1, resign the two parts separately and safely
    value = -2*half + value; // Cannot bitshift half, as it may be at the limit of the bits JS uses in bitshifts
  }
  return value;
}

// check for imul support, and also for correctness ( https://bugs.webkit.org/show_bug.cgi?id=126345 )
if (!Math['imul'] || Math['imul'](0xffffffff, 5) !== -5) Math['imul'] = function imul(a, b) {
  var ah  = a >>> 16;
  var al = a & 0xffff;
  var bh  = b >>> 16;
  var bl = b & 0xffff;
  return (al*bl + ((ah*bl + al*bh) << 16))|0;
};
Math.imul = Math['imul'];


var Math_abs = Math.abs;
var Math_cos = Math.cos;
var Math_sin = Math.sin;
var Math_tan = Math.tan;
var Math_acos = Math.acos;
var Math_asin = Math.asin;
var Math_atan = Math.atan;
var Math_atan2 = Math.atan2;
var Math_exp = Math.exp;
var Math_log = Math.log;
var Math_sqrt = Math.sqrt;
var Math_ceil = Math.ceil;
var Math_floor = Math.floor;
var Math_pow = Math.pow;
var Math_imul = Math.imul;
var Math_fround = Math.fround;
var Math_min = Math.min;

// A counter of dependencies for calling run(). If we need to
// do asynchronous work before running, increment this and
// decrement it. Incrementing must happen in a place like
// PRE_RUN_ADDITIONS (used by emcc to add file preloading).
// Note that you can add dependencies in preRun, even though
// it happens right before run - run will be postponed until
// the dependencies are met.
var runDependencies = 0;
var runDependencyWatcher = null;
var dependenciesFulfilled = null; // overridden to take different actions when all run dependencies are fulfilled

function addRunDependency(id) {
  runDependencies++;
  if (Module['monitorRunDependencies']) {
    Module['monitorRunDependencies'](runDependencies);
  }
}
Module['addRunDependency'] = addRunDependency;
function removeRunDependency(id) {
  runDependencies--;
  if (Module['monitorRunDependencies']) {
    Module['monitorRunDependencies'](runDependencies);
  }
  if (runDependencies == 0) {
    if (runDependencyWatcher !== null) {
      clearInterval(runDependencyWatcher);
      runDependencyWatcher = null;
    }
    if (dependenciesFulfilled) {
      var callback = dependenciesFulfilled;
      dependenciesFulfilled = null;
      callback(); // can add another dependenciesFulfilled
    }
  }
}
Module['removeRunDependency'] = removeRunDependency;

Module["preloadedImages"] = {}; // maps url to image data
Module["preloadedAudios"] = {}; // maps url to audio data


var memoryInitializer = null;

// === Body ===



STATIC_BASE = 8;

STATICTOP = STATIC_BASE + 9960;


/* global initializers */ __ATINIT__.push({ func: function() { runPostSets() } },{ func: function() { __GLOBAL__I_a() } });

































































































































































































































































var __ZTVN10__cxxabiv120__si_class_type_infoE;
__ZTVN10__cxxabiv120__si_class_type_infoE=allocate([0,0,0,0,176,30,0,0,40,0,0,0,216,0,0,0,198,0,0,0,62,0,0,0,4,0,0,0,8,0,0,0,2,0,0,0,18,0,0,0,0,0,0,0,0,0,0,0], "i8", ALLOC_STATIC);;
var __ZTVN10__cxxabiv117__class_type_infoE;
__ZTVN10__cxxabiv117__class_type_infoE=allocate([0,0,0,0,192,30,0,0,40,0,0,0,58,0,0,0,198,0,0,0,62,0,0,0,4,0,0,0,2,0,0,0,4,0,0,0,10,0,0,0,0,0,0,0,0,0,0,0], "i8", ALLOC_STATIC);;

























































































































































































































































































var __ZN12b2ChainShapeD1Ev;
var __ZN12b2BroadPhaseC1Ev;
var __ZN12b2BroadPhaseD1Ev;
var __ZN13b2DynamicTreeC1Ev;
var __ZN13b2DynamicTreeD1Ev;
var __ZN8b2IslandC1EiiiP16b2StackAllocatorP17b2ContactListener;
var __ZN8b2IslandD1Ev;
var __ZN12b2MouseJointC1EPK15b2MouseJointDef;
var __ZN12b2MotorJointC1EPK15b2MotorJointDef;
var __ZN15b2DistanceJointC1EPK18b2DistanceJointDef;
var __ZN15b2FrictionJointC1EPK18b2FrictionJointDef;
var __ZN11b2WeldJointC1EPK14b2WeldJointDef;
var __ZN11b2GearJointC1EPK14b2GearJointDef;
var __ZN16b2PrismaticJointC1EPK19b2PrismaticJointDef;
var __ZN11b2RopeJointC1EPK14b2RopeJointDef;
var __ZN15b2RevoluteJointC1EPK18b2RevoluteJointDef;
var __ZN12b2WheelJointC1EPK15b2WheelJointDef;
var __ZN13b2PulleyJointC1EPK16b2PulleyJointDef;
var __ZN15b2ContactSolverC1EP18b2ContactSolverDef;
var __ZN15b2ContactSolverD1Ev;
var __ZN9b2FixtureC1Ev;
var __ZN7b2WorldC1ERK6b2Vec2;
var __ZN7b2WorldD1Ev;
var __ZN6b2BodyC1EPK9b2BodyDefP7b2World;
var __ZN6b2BodyD1Ev;
var __ZN16b2ContactManagerC1Ev;
var __ZN16b2VoronoiDiagramC1EP16b2StackAllocatori;
var __ZN16b2VoronoiDiagramD1Ev;
var __ZN15b2ParticleGroupC1Ev;
var __ZN16b2ParticleSystemC1EPK19b2ParticleSystemDefP7b2World;
var __ZN16b2ParticleSystemD1Ev;
var __ZN16b2BlockAllocatorC1Ev;
var __ZN16b2BlockAllocatorD1Ev;
var __ZN7b2TimerC1Ev;
var __ZN16b2StackAllocatorC1Ev;
var __ZN16b2StackAllocatorD1Ev;
/* memory initializer */ allocate([0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,240,6,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,19,0,0,0,0,0,0,128,20,0,0,0,0,0,0,32,32,106,100,46,108,101,110,103,116,104,66,32,61,32,37,46,49,53,108,101,102,59,10,0,0,0,0,0,0,0,0,32,32,106,100,46,109,97,120,77,111,116,111,114,84,111,114,113,117,101,32,61,32,37,46,49,53,108,101,102,59,10,0,32,32,106,100,46,117,112,112,101,114,65,110,103,108,101,32,61,32,37,46,49,53,108,101,102,59,10,0,0,0,0,0,32,32,106,100,46,108,111,119,101,114,84,114,97,110,115,108,97,116,105,111,110,32,61,32,37,46,49,53,108,101,102,59,10,0,0,0,0,0,0,0,32,32,106,111,105,110,116,115,91,37,100,93,32,61,32,109,95,119,111,114,108,100,45,62,67,114,101,97,116,101,74,111,105,110,116,40,38,106,100,41,59,10,0,0,0,0,0,0,32,32,106,111,105,110,116,115,91,37,100,93,32,61,32,109,95,119,111,114,108,100,45,62,67,114,101,97,116,101,74,111,105,110,116,40,38,106,100,41,59,10,0,0,0,0,0,0,32,32,106,111,105,110,116,115,91,37,100,93,32,61,32,109,95,119,111,114,108,100,45,62,67,114,101,97,116,101,74,111,105,110,116,40,38,106,100,41,59,10,0,0,0,0,0,0,32,32,106,100,46,108,101,110,103,116,104,65,32,61,32,37,46,49,53,108,101,102,59,10,0,0,0,0,0,0,0,0,32,32,106,100,46,109,111,116,111,114,83,112,101,101,100,32,61,32,37,46,49,53,108,101,102,59,10,0,0,0,0,0,32,32,106,100,46,108,111,119,101,114,65,110,103,108,101,32,61,32,37,46,49,53,108,101,102,59,10,0,0,0,0,0,32,32,106,100,46,101,110,97,98,108,101,76,105,109,105,116,32,61,32,98,111,111,108,40,37,100,41,59,10,0,0,0,32,32,106,100,46,100,97,109,112,105,110,103,82,97,116,105,111,32,61,32,37,46,49,53,108,101,102,59,10,0,0,0,32,32,106,111,105,110,116,115,91,37,100,93,32,61,32,109,95,119,111,114,108,100,45,62,67,114,101,97,116,101,74,111,105,110,116,40,38,106,100,41,59,10,0,0,0,0,0,0,32,32,106,100,46,100,97,109,112,105,110,103,82,97,116,105,111,32,61,32,37,46,49,53,108,101,102,59,10,0,0,0,32,32,106,100,46,99,111,114,114,101,99,116,105,111,110,70,97,99,116,111,114,32,61,32,37,46,49,53,108,101,102,59,10,0,0,0,0,0,0,0,32,32,106,100,46,108,111,99,97,108,65,110,99,104,111,114,66,46,83,101,116,40,37,46,49,53,108,101,102,44,32,37,46,49,53,108,101,102,41,59,10,0,0,0,0,0,0,0,32,32,106,100,46,101,110,97,98,108,101,77,111,116,111,114,32,61,32,98,111,111,108,40,37,100,41,59,10,0,0,0,32,32,106,100,46,101,110,97,98,108,101,76,105,109,105,116,32,61,32,98,111,111,108,40,37,100,41,59,10,0,0,0,32,32,106,111,105,110,116,115,91,37,100,93,32,61,32,109,95,119,111,114,108,100,45,62,67,114,101,97,116,101,74,111,105,110,116,40,38,106,100,41,59,10,0,0,0,0,0,0,32,32,106,100,46,114,101,102,101,114,101,110,99,101,65,110,103,108,101,32,61,32,37,46,49,53,108,101,102,59,10,0,32,32,106,111,105,110,116,115,91,37,100,93,32,61,32,109,95,119,111,114,108,100,45,62,67,114,101,97,116,101,74,111,105,110,116,40,38,106,100,41,59,10,0,0,0,0,0,0,32,32,106,100,46,102,114,101,113,117,101,110,99,121,72,122,32,61,32,37,46,49,53,108,101,102,59,10,0,0,0,0,32,32,106,100,46,109,97,120,84,111,114,113,117,101,32,61,32,37,46,49,53,108,101,102,59,10,0,0,0,0,0,0,32,32,106,100,46,102,114,101,113,117,101,110,99,121,72,122,32,61,32,37,46,49,53,108,101,102,59,10,0,0,0,0,32,32,106,100,46,109,97,120,84,111,114,113,117,101,32,61,32,37,46,49,53,108,101,102,59,10,0,0,0,0,0,0,32,32,106,100,46,108,111,99,97,108,65,110,99,104,111,114,65,46,83,101,116,40,37,46,49,53,108,101,102,44,32,37,46,49,53,108,101,102,41,59,10,0,0,0,0,0,0,0,32,32,106,100,46,108,111,99,97,108,65,120,105,115,65,46,83,101,116,40,37,46,49,53,108,101,102,44,32,37,46,49,53,108,101,102,41,59,10,0,32,32,106,100,46,114,101,102,101,114,101,110,99,101,65,110,103,108,101,32,61,32,37,46,49,53,108,101,102,59,10,0,32,32,106,100,46,109,97,120,76,101,110,103,116,104,32,61,32,37,46,49,53,108,101,102,59,10,0,0,0,0,0,0,32,32,106,100,46,108,111,99,97,108,65,120,105,115,65,46,83,101,116,40,37,46,49,53,108,101,102,44,32,37,46,49,53,108,101,102,41,59,10,0,32,32,106,100,46,114,97,116,105,111,32,61,32,37,46,49,53,108,101,102,59,10,0,0,32,32,106,100,46,114,101,102,101,114,101,110,99,101,65,110,103,108,101,32,61,32,37,46,49,53,108,101,102,59,10,0,32,32,106,100,46,109,97,120,70,111,114,99,101,32,61,32,37,46,49,53,108,101,102,59,10,0,0,0,0,0,0,0,32,32,106,100,46,108,101,110,103,116,104,32,61,32,37,46,49,53,108,101,102,59,10,0,32,32,106,100,46,109,97,120,70,111,114,99,101,32,61,32,37,46,49,53,108,101,102,59,10,0,0,0,0,0,0,0,32,32,106,100,46,103,114,111,117,110,100,65,110,99,104,111,114,66,46,83,101,116,40,37,46,49,53,108,101,102,44,32,37,46,49,53,108,101,102,41,59,10,0,0,0,0,0,0,32,32,106,100,46,108,111,99,97,108,65,110,99,104,111,114,66,46,83,101,116,40,37,46,49,53,108,101,102,44,32,37,46,49,53,108,101,102,41,59,10,0,0,0,0,0,0,0,32,32,106,100,46,108,111,99,97,108,65,110,99,104,111,114,66,46,83,101,116,40,37,46,49,53,108,101,102,44,32,37,46,49,53,108,101,102,41,59,10,0,0,0,0,0,0,0,32,32,106,100,46,108,111,99,97,108,65,110,99,104,111,114,66,46,83,101,116,40,37,46,49,53,108,101,102,44,32,37,46,49,53,108,101,102,41,59,10,0,0,0,0,0,0,0,32,32,106,100,46,108,111,99,97,108,65,110,99,104,111,114,66,46,83,101,116,40,37,46,49,53,108,101,102,44,32,37,46,49,53,108,101,102,41,59,10,0,0,0,0,0,0,0,32,32,106,100,46,106,111,105,110,116,50,32,61,32,106,111,105,110,116,115,91,37,100,93,59,10,0,0,0,0,0,0,32,32,106,100,46,108,111,99,97,108,65,110,99,104,111,114,66,46,83,101,116,40,37,46,49,53,108,101,102,44,32,37,46,49,53,108,101,102,41,59,10,0,0,0,0,0,0,0,32,32,106,100,46,108,111,99,97,108,65,110,99,104,111,114,66,46,83,101,116,40,37,46,49,53,108,101,102,44,32,37,46,49,53,108,101,102,41,59,10,0,0,0,0,0,0,0,32,32,106,100,46,108,111,99,97,108,65,110,99,104,111,114,66,46,83,101,116,40,37,46,49,53,108,101,102,44,32,37,46,49,53,108,101,102,41,59,10,0,0,0,0,0,0,0,32,32,106,100,46,97,110,103,117,108,97,114,79,102,102,115,101,116,32,61,32,37,46,49,53,108,101,102,59,10,0,0,76,105,113,117,105,100,70,117,110,32,49,46,48,46,48,0,32,32,106,100,46,103,114,111,117,110,100,65,110,99,104,111,114,65,46,83,101,116,40,37,46,49,53,108,101,102,44,32,37,46,49,53,108,101,102,41,59,10,0,0,0,0,0,0,32,32,106,100,46,108,111,99,97,108,65,110,99,104,111,114,65,46,83,101,116,40,37,46,49,53,108,101,102,44,32,37,46,49,53,108,101,102,41,59,10,0,0,0,0,0,0,0,32,32,106,100,46,108,111,99,97,108,65,110,99,104,111,114,65,46,83,101,116,40,37,46,49,53,108,101,102,44,32,37,46,49,53,108,101,102,41,59,10,0,0,0,0,0,0,0,32,32,106,100,46,108,111,99,97,108,65,110,99,104,111,114,65,46,83,101,116,40,37,46,49,53,108,101,102,44,32,37,46,49,53,108,101,102,41,59,10,0,0,0,0,0,0,0,32,32,106,100,46,108,111,99,97,108,65,110,99,104,111,114,65,46,83,101,116,40,37,46,49,53,108,101,102,44,32,37,46,49,53,108,101,102,41,59,10,0,0,0,0,0,0,0,32,32,106,100,46,106,111,105,110,116,49,32,61,32,106,111,105,110,116,115,91,37,100,93,59,10,0,0,0,0,0,0,32,32,106,100,46,108,111,99,97,108,65,110,99,104,111,114,65,46,83,101,116,40,37,46,49,53,108,101,102,44,32,37,46,49,53,108,101,102,41,59,10,0,0,0,0,0,0,0,32,32,106,100,46,108,111,99,97,108,65,110,99,104,111,114,65,46,83,101,116,40,37,46,49,53,108,101,102,44,32,37,46,49,53,108,101,102,41,59,10,0,0,0,0,0,0,0,32,32,106,100,46,108,111,99,97,108,65,110,99,104,111,114,65,46,83,101,116,40,37,46,49,53,108,101,102,44,32,37,46,49,53,108,101,102,41,59,10,0,0,0,0,0,0,0,32,32,106,100,46,108,105,110,101,97,114,79,102,102,115,101,116,46,83,101,116,40,37,46,49,53,108,101,102,44,32,37,46,49,53,108,101,102,41,59,10,0,0,0,0,0,0,0,115,116,100,58,58,98,97,100,95,97,108,108,111,99,0,0,32,32,106,100,46,99,111,108,108,105,100,101,67,111,110,110,101,99,116,101,100,32,61,32,98,111,111,108,40,37,100,41,59,10,0,0,0,0,0,0,32,32,106,100,46,99,111,108,108,105,100,101,67,111,110,110,101,99,116,101,100,32,61,32,98,111,111,108,40,37,100,41,59,10,0,0,0,0,0,0,32,32,106,100,46,99,111,108,108,105,100,101,67,111,110,110,101,99,116,101,100,32,61,32,98,111,111,108,40,37,100,41,59,10,0,0,0,0,0,0,32,32,106,100,46,99,111,108,108,105,100,101,67,111,110,110,101,99,116,101,100,32,61,32,98,111,111,108,40,37,100,41,59,10,0,0,0,0,0,0,32,32,106,100,46,99,111,108,108,105,100,101,67,111,110,110,101,99,116,101,100,32,61,32,98,111,111,108,40,37,100,41,59,10,0,0,0,0,0,0,32,32,106,100,46,99,111,108,108,105,100,101,67,111,110,110,101,99,116,101,100,32,61,32,98,111,111,108,40,37,100,41,59,10,0,0,0,0,0,0,32,32,106,100,46,99,111,108,108,105,100,101,67,111,110,110,101,99,116,101,100,32,61,32,98,111,111,108,40,37,100,41,59,10,0,0,0,0,0,0,32,32,106,100,46,99,111,108,108,105,100,101,67,111,110,110,101,99,116,101,100,32,61,32,98,111,111,108,40,37,100,41,59,10,0,0,0,0,0,0,32,32,106,100,46,99,111,108,108,105,100,101,67,111,110,110,101,99,116,101,100,32,61,32,98,111,111,108,40,37,100,41,59,10,0,0,0,0,0,0,32,32,106,100,46,99,111,108,108,105,100,101,67,111,110,110,101,99,116,101,100,32,61,32,98,111,111,108,40,37,100,41,59,10,0,0,0,0,0,0,32,32,98,50,80,117,108,108,101,121,74,111,105,110,116,68,101,102,32,106,100,59,10,0,32,32,98,50,87,104,101,101,108,74,111,105,110,116,68,101,102,32,106,100,59,10,0,0,32,32,106,100,46,98,111,100,121,66,32,61,32,98,111,100,105,101,115,91,37,100,93,59,10,0,0,0,0,0,0,0,32,32,106,100,46,98,111,100,121,66,32,61,32,98,111,100,105,101,115,91,37,100,93,59,10,0,0,0,0,0,0,0,32,32,106,100,46,98,111,100,121,66,32,61,32,98,111,100,105,101,115,91,37,100,93,59,10,0,0,0,0,0,0,0,32,32,106,100,46,98,111,100,121,66,32,61,32,98,111,100,105,101,115,91,37,100,93,59,10,0,0,0,0,0,0,0,32,32,106,100,46,98,111,100,121,66,32,61,32,98,111,100,105,101,115,91,37,100,93,59,10,0,0,0,0,0,0,0,32,32,106,100,46,98,111,100,121,66,32,61,32,98,111,100,105,101,115,91,37,100,93,59,10,0,0,0,0,0,0,0,32,32,106,100,46,98,111,100,121,66,32,61,32,98,111,100,105,101,115,91,37,100,93,59,10,0,0,0,0,0,0,0,32,32,106,100,46,98,111,100,121,66,32,61,32,98,111,100,105,101,115,91,37,100,93,59,10,0,0,0,0,0,0,0,32,32,106,100,46,98,111,100,121,66,32,61,32,98,111,100,105,101,115,91,37,100,93,59,10,0,0,0,0,0,0,0,32,32,106,100,46,98,111,100,121,66,32,61,32,98,111,100,105,101,115,91,37,100,93,59,10,0,0,0,0,0,0,0,32,32,98,50,82,101,118,111,108,117,116,101,74,111,105,110,116,68,101,102,32,106,100,59,10,0,0,0,0,0,0,0,47,47,32,68,117,109,112,32,105,115,32,110,111,116,32,115,117,112,112,111,114,116,101,100,32,102,111,114,32,116,104,105,115,32,106,111,105,110,116,32,116,121,112,101,46,10,0,0,32,32,98,50,82,111,112,101,74,111,105,110,116,68,101,102,32,106,100,59,10,0,0,0,32,32,98,50,80,114,105,115,109,97,116,105,99,74,111,105,110,116,68,101,102,32,106,100,59,10,0,0,0,0,0,0,32,32,98,50,71,101,97,114,74,111,105,110,116,68,101,102,32,106,100,59,10,0,0,0,32,32,106,111,105,110,116,115,91,37,100,93,32,61,32,109,95,119,111,114,108,100,45,62,67,114,101,97,116,101,74,111,105,110,116,40,38,106,100,41,59,10,0,0,0,0,0,0,32,32,98,50,87,101,108,100,74,111,105,110,116,68,101,102,32,106,100,59,10,0,0,0,32,32,106,111,105,110,116,115,91,37,100,93,32,61,32,109,95,119,111,114,108,100,45,62,67,114,101,97,116,101,74,111,105,110,116,40,38,106,100,41,59,10,0,0,0,0,0,0,32,32,106,100,46,109,97,120,77,111,116,111,114,70,111,114,99,101,32,61,32,37,46,49,53,108,101,102,59,10,0,0,32,32,98,50,70,114,105,99,116,105,111,110,74,111,105,110,116,68,101,102,32,106,100,59,10,0,0,0,0,0,0,0,32,32,106,100,46,98,111,100,121,65,32,61,32,98,111,100,105,101,115,91,37,100,93,59,10,0,0,0,0,0,0,0,32,32,106,100,46,98,111,100,121,65,32,61,32,98,111,100,105,101,115,91,37,100,93,59,10,0,0,0,0,0,0,0,32,32,106,111,105,110,116,115,91,37,100,93,32,61,32,109,95,119,111,114,108,100,45,62,67,114,101,97,116,101,74,111,105,110,116,40,38,106,100,41,59,10,0,0,0,0,0,0,32,32,106,100,46,109,97,120,77,111,116,111,114,84,111,114,113,117,101,32,61,32,37,46,49,53,108,101,102,59,10,0,32,32,106,100,46,109,111,116,111,114,83,112,101,101,100,32,61,32,37,46,49,53,108,101,102,59,10,0,0,0,0,0,32,32,106,100,46,98,111,100,121,65,32,61,32,98,111,100,105,101,115,91,37,100,93,59,10,0,0,0,0,0,0,0,32,32,106,100,46,98,111,100,121,65,32,61,32,98,111,100,105,101,115,91,37,100,93,59,10,0,0,0,0,0,0,0,32,32,106,100,46,98,111,100,121,65,32,61,32,98,111,100,105,101,115,91,37,100,93,59,10,0,0,0,0,0,0,0,32,32,106,100,46,98,111,100,121,65,32,61,32,98,111,100,105,101,115,91,37,100,93,59,10,0,0,0,0,0,0,0,32,32,98,50,68,105,115,116,97,110,99,101,74,111,105,110,116,68,101,102,32,106,100,59,10,0,0,0,0,0,0,0,32,32,106,100,46,98,111,100,121,65,32,61,32,98,111,100,105,101,115,91,37,100,93,59,10,0,0,0,0,0,0,0,32,32,106,100,46,98,111,100,121,65,32,61,32,98,111,100,105,101,115,91,37,100,93,59,10,0,0,0,0,0,0,0,32,32,106,111,105,110,116,115,91,37,100,93,32,61,32,109,95,119,111,114,108,100,45,62,67,114,101,97,116,101,74,111,105,110,116,40,38,106,100,41,59,10,0,0,0,0,0,0,32,32,106,100,46,100,97,109,112,105,110,103,82,97,116,105,111,32,61,32,37,46,49,53,108,101,102,59,10,0,0,0,32,32,106,100,46,109,111,116,111,114,83,112,101,101,100,32,61,32,37,46,49,53,108,101,102,59,10,0,0,0,0,0,32,32,106,100,46,98,111,100,121,65,32,61,32,98,111,100,105,101,115,91,37,100,93,59,10,0,0,0,0,0,0,0,32,32,106,100,46,101,110,97,98,108,101,77,111,116,111,114,32,61,32,98,111,111,108,40,37,100,41,59,10,0,0,0,32,32,98,50,77,111,116,111,114,74,111,105,110,116,68,101,102,32,106,100,59,10,0,0,32,32,106,100,46,114,97,116,105,111,32,61,32,37,46,49,53,108,101,102,59,10,0,0,32,32,106,100,46,102,114,101,113,117,101,110,99,121,72,122,32,61,32,37,46,49,53,108,101,102,59,10,0,0,0,0,32,32,106,100,46,101,110,97,98,108,101,77,111,116,111,114,32,61,32,98,111,111,108,40,37,100,41,59,10,0,0,0,32,32,106,100,46,117,112,112,101,114,84,114,97,110,115,108,97,116,105,111,110,32,61,32,37,46,49,53,108,101,102,59,10,0,0,0,0,0,0,0,32,32,106,100,46,98,111,100,121,65,32,61,32,98,111,100,105,101,115,91,37,100,93,59,10,0,0,0,0,0,0,0,77,111,117,115,101,32,106,111,105,110,116,32,100,117,109,112,105,110,103,32,105,115,32,110,111,116,32,115,117,112,112,111,114,116,101,100,46,10,0,0,0,0,0,0,16,30,0,0,134,0,0,0,122,0,0,0,12,0,0,0,2,0,0,0,4,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,32,30,0,0,82,0,0,0,170,0,0,0,20,0,0,0,8,0,0,0,18,0,0,0,6,0,0,0,12,0,0,0,6,0,0,0,18,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,48,30,0,0,156,0,0,0,38,0,0,0,16,0,0,0,6,0,0,0,24,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,64,30,0,0,10,0,0,0,6,0,0,0,32,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,80,30,0,0,120,0,0,0,2,0,0,0,4,0,0,0,8,0,0,0,36,0,0,0,30,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,96,30,0,0,200,0,0,0,84,0,0,0,50,0,0,0,14,0,0,0,2,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,112,30,0,0,100,0,0,0,196,0,0,0,4,0,0,0,8,0,0,0,36,0,0,0,14,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,144,30,0,0,90,0,0,0,174,0,0,0,2,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,168,30,0,0,192,0,0,0,154,0,0,0,50,0,0,0,2,0,0,0,4,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,224,30,0,0,2,0,0,0,48,0,0,0,102,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,240,30,0,0,2,0,0,0,2,0,0,0,2,0,0,0,2,0,0,0,108,0,0,0,90,0,0,0,212,0,0,0,28,0,0,0,2,0,0,0,2,0,0,0,2,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,8,31,0,0,16,0,0,0,34,0,0,0,8,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,24,31,0,0,4,0,0,0,118,0,0,0,166,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,40,31,0,0,26,0,0,0,110,0,0,0,66,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,56,31,0,0,38,0,0,0,74,0,0,0,132,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,72,31,0,0,60,0,0,0,180,0,0,0,64,0,0,0,38,0,0,0,12,0,0,0,20,0,0,0,2,0,0,0,36,0,0,0,4,0,0,0,6,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,88,31,0,0,34,0,0,0,68,0,0,0,12,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,112,31,0,0,80,0,0,0,24,0,0,0,28,0,0,0,44,0,0,0,12,0,0,0,20,0,0,0,2,0,0,0,36,0,0,0,16,0,0,0,8,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,120,31,0,0,98,0,0,0,20,0,0,0,26,0,0,0,8,0,0,0,24,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,136,31,0,0,66,0,0,0,108,0,0,0,24,0,0,0,16,0,0,0,14,0,0,0,90,0,0,0,142,0,0,0,52,0,0,0,12,0,0,0,20,0,0,0,42,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,152,31,0,0,12,0,0,0,86,0,0,0,126,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,168,31,0,0,14,0,0,0,24,0,0,0,4,0,0,0,2,0,0,0,70,0,0,0,90,0,0,0,88,0,0,0,202,0,0,0,128,0,0,0,124,0,0,0,30,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,192,31,0,0,106,0,0,0,98,0,0,0,28,0,0,0,10,0,0,0,136,0,0,0,90,0,0,0,76,0,0,0,32,0,0,0,132,0,0,0,10,0,0,0,2,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,208,31,0,0,130,0,0,0,16,0,0,0,16,0,0,0,12,0,0,0,106,0,0,0,90,0,0,0,182,0,0,0,44,0,0,0,70,0,0,0,32,0,0,0,46,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,224,31,0,0,176,0,0,0,116,0,0,0,16,0,0,0,8,0,0,0,6,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,232,31,0,0,8,0,0,0,138,0,0,0,22,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,248,31,0,0,16,0,0,0,218,0,0,0,2,0,0,0,2,0,0,0,18,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,8,32,0,0,78,0,0,0,140,0,0,0,14,0,0,0,4,0,0,0,10,0,0,0,12,0,0,0,6,0,0,0,2,0,0,0,6,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,24,32,0,0,86,0,0,0,22,0,0,0,22,0,0,0,20,0,0,0,130,0,0,0,40,0,0,0,128,0,0,0,72,0,0,0,62,0,0,0,84,0,0,0,6,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,40,32,0,0,220,0,0,0,164,0,0,0,22,0,0,0,6,0,0,0,22,0,0,0,10,0,0,0,22,0,0,0,28,0,0,0,20,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,56,32,0,0,136,0,0,0,134,0,0,0,10,0,0,0,4,0,0,0,188,0,0,0,90,0,0,0,4,0,0,0,64,0,0,0,80,0,0,0,54,0,0,0,52,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,72,32,0,0,2,0,0,0,126,0,0,0,8,0,0,0,6,0,0,0,56,0,0,0,8,0,0,0,162,0,0,0,206,0,0,0,56,0,0,0,18,0,0,0,34,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,88,32,0,0,58,0,0,0,88,0,0,0,12,0,0,0,8,0,0,0,214,0,0,0,90,0,0,0,186,0,0,0,114,0,0,0,4,0,0,0,142,0,0,0,56,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,104,32,0,0,150,0,0,0,178,0,0,0,58,0,0,0,10,0,0,0,20,0,0,0,16,0,0,0,16,0,0,0,22,0,0,0,2,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,120,32,0,0,112,0,0,0,100,0,0,0,14,0,0,0,18,0,0,0,96,0,0,0,90,0,0,0,194,0,0,0,30,0,0,0,6,0,0,0,118,0,0,0,60,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,136,32,0,0,82,0,0,0,74,0,0,0,30,0,0,0,22,0,0,0,204,0,0,0,90,0,0,0,36,0,0,0,94,0,0,0,138,0,0,0,36,0,0,0,44,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,152,32,0,0,68,0,0,0,26,0,0,0,26,0,0,0,14,0,0,0,26,0,0,0,90,0,0,0,184,0,0,0,148,0,0,0,140,0,0,0,42,0,0,0,62,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,168,32,0,0,208,0,0,0,42,0,0,0,54,0,0,0,12,0,0,0,12,0,0,0,4,0,0,0,20,0,0,0,24,0,0,0,32,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,90,78,49,54,98,50,80,97,114,116,105,99,108,101,83,121,115,116,101,109,52,49,85,112,100,97,116,101,80,97,105,114,115,65,110,100,84,114,105,97,100,115,87,105,116,104,82,101,97,99,116,105,118,101,80,97,114,116,105,99,108,101,115,69,118,69,49,52,82,101,97,99,116,105,118,101,70,105,108,116,101,114,0,0,0,0,0,0,90,78,49,54,98,50,80,97,114,116,105,99,108,101,83,121,115,116,101,109,51,51,67,114,101,97,116,101,80,97,114,116,105,99,108,101,115,87,105,116,104,83,104,97,112,101,115,70,111,114,71,114,111,117,112,69,80,75,80,75,55,98,50,83,104,97,112,101,105,82,75,49,56,98,50,80,97,114,116,105,99,108,101,71,114,111,117,112,68,101,102,82,75,49,49,98,50,84,114,97,110,115,102,111,114,109,69,49,52,67,111,109,112,111,115,105,116,101,83,104,97,112,101,0,0,0,0,0,90,78,49,54,98,50,80,97,114,116,105,99,108,101,83,121,115,116,101,109,50,51,68,101,115,116,114,111,121,80,97,114,116,105,99,108,101,115,73,110,83,104,97,112,101,69,82,75,55,98,50,83,104,97,112,101,82,75,49,49,98,50,84,114,97,110,115,102,111,114,109,98,69,51,49,68,101,115,116,114,111,121,80,97,114,116,105,99,108,101,115,73,110,83,104,97,112,101,67,97,108,108,98,97,99,107,0,0,0,0,0,0,90,78,49,54,98,50,80,97,114,116,105,99,108,101,83,121,115,116,101,109,50,48,85,112,100,97,116,101,80,97,105,114,115,65,110,100,84,114,105,97,100,115,69,105,105,82,75,78,83,95,49,54,67,111,110,110,101,99,116,105,111,110,70,105,108,116,101,114,69,69,50,48,85,112,100,97,116,101,84,114,105,97,100,115,67,97,108,108,98,97,99,107,0,0,0,0,90,78,49,54,98,50,80,97,114,116,105,99,108,101,83,121,115,116,101,109,49,56,85,112,100,97,116,101,66,111,100,121,67,111,110,116,97,99,116,115,69,118,69,50,54,85,112,100,97,116,101,66,111,100,121,67,111,110,116,97,99,116,115,67,97,108,108,98,97,99,107,0,90,78,49,54,98,50,80,97,114,116,105,99,108,101,83,121,115,116,101,109,49,56,74,111,105,110,80,97,114,116,105,99,108,101,71,114,111,117,112,115,69,80,49,53,98,50,80,97,114,116,105,99,108,101,71,114,111,117,112,83,49,95,69,50,52,74,111,105,110,80,97,114,116,105,99,108,101,71,114,111,117,112,115,70,105,108,116,101,114,0,0,0,0,0,0,0,90,78,49,54,98,50,80,97,114,116,105,99,108,101,83,121,115,116,101,109,49,52,83,111,108,118,101,67,111,108,108,105,115,105,111,110,69,82,75,49,48,98,50,84,105,109,101,83,116,101,112,69,50,50,83,111,108,118,101,67,111,108,108,105,115,105,111,110,67,97,108,108,98,97,99,107,0,0,0,0,83,116,57,116,121,112,101,95,105,110,102,111,0,0,0,0,83,116,57,101,120,99,101,112,116,105,111,110,0,0,0,0,83,116,57,98,97,100,95,97,108,108,111,99,0,0,0,0,78,49,54,98,50,86,111,114,111,110,111,105,68,105,97,103,114,97,109,49,50,78,111,100,101,67,97,108,108,98,97,99,107,69,0,0,0,0,0,0,78,49,54,98,50,80,97,114,116,105,99,108,101,83,121,115,116,101,109,49,54,67,111,110,110,101,99,116,105,111,110,70,105,108,116,101,114,69,0,0,78,49,48,95,95,99,120,120,97,98,105,118,49,50,48,95,95,115,105,95,99,108,97,115,115,95,116,121,112,101,95,105,110,102,111,69,0,0,0,0,78,49,48,95,95,99,120,120,97,98,105,118,49,49,55,95,95,99,108,97,115,115,95,116,121,112,101,95,105,110,102,111,69,0,0,0,0,0,0,0,78,49,48,95,95,99,120,120,97,98,105,118,49,49,54,95,95,115,104,105,109,95,116,121,112,101,95,105,110,102,111,69,0,0,0,0,0,0,0,0,57,98,50,67,111,110,116,97,99,116,0,0,0,0,0,0,55,98,50,83,104,97,112,101,0,0,0,0,0,0,0,0,55,98,50,74,111,105,110,116,0,0,0,0,0,0,0,0,51,48,98,50,70,105,120,116,117,114,101,80,97,114,116,105,99,108,101,81,117,101,114,121,67,97,108,108,98,97,99,107,0,0,0,0,0,0,0,0,50,53,98,50,80,111,108,121,103,111,110,65,110,100,67,105,114,99,108,101,67,111,110,116,97,99,116,0,0,0,0,0,50,52,98,50,67,104,97,105,110,65,110,100,80,111,108,121,103,111,110,67,111,110,116,97,99,116,0,0,0,0,0,0,50,51,98,50,69,100,103,101,65,110,100,80,111,108,121,103,111,110,67,111,110,116,97,99,116,0,0,0,0,0,0,0,50,51,98,50,67,104,97,105,110,65,110,100,67,105,114,99,108,101,67,111,110,116,97,99,116,0,0,0,0,0,0,0,50,50,98,50,87,111,114,108,100,67,111,110,116,97,99,116,76,105,115,116,101,110,101,114,0,0,0,0,0,0,0,0,50,50,98,50,69,100,103,101,65,110,100,67,105,114,99,108,101,67,111,110,116,97,99,116,0,0,0,0,0,0,0,0,49,55,98,50,82,97,121,67,97,115,116,67,97,108,108,98,97,99,107,0,0,0,0,0,49,55,98,50,67,111,110,116,97,99,116,76,105,115,116,101,110,101,114,0,0,0,0,0,49,55,81,117,101,114,121,65,65,66,66,67,97,108,108,98,97,99,107,0,0,0,0,0,49,54,98,50,80,114,105,115,109,97,116,105,99,74,111,105,110,116,0,0,0,0,0,0,49,54,98,50,80,111,108,121,103,111,110,67,111,110,116,97,99,116,0,0,0,0,0,0,49,53,98,50,82,101,118,111,108,117,116,101,74,111,105,110,116,0,0,0,0,0,0,0,49,53,98,50,81,117,101,114,121,67,97,108,108,98,97,99,107,0,0,0,0,0,0,0,49,53,98,50,70,114,105,99,116,105,111,110,74,111,105,110,116,0,0,0,0,0,0,0,49,53,98,50,68,105,115,116,97,110,99,101,74,111,105,110,116,0,0,0,0,0,0,0,49,53,98,50,67,111,110,116,97,99,116,70,105,108,116,101,114,0,0,0,0,0,0,0,49,53,98,50,67,105,114,99,108,101,67,111,110,116,97,99,116,0,0,0,0,0,0,0,49,53,82,97,121,67,97,115,116,67,97,108,108,98,97,99,107,0,0,0,0,0,0,0,49,52,98,50,80,111,108,121,103,111,110,83,104,97,112,101,0,0,0,0,0,0,0,0,49,51,98,50,80,117,108,108,101,121,74,111,105,110,116,0,49,51,98,50,67,105,114,99,108,101,83,104,97,112,101,0,49,50,98,50,87,104,101,101,108,74,111,105,110,116,0,0,49,50,98,50,77,111,117,115,101,74,111,105,110,116,0,0,49,50,98,50,77,111,116,111,114,74,111,105,110,116,0,0,49,50,98,50,67,104,97,105,110,83,104,97,112,101,0,0,49,49,98,50,87,101,108,100,74,111,105,110,116,0,0,0,49,49,98,50,82,111,112,101,74,111,105,110,116,0,0,0,49,49,98,50,71,101,97,114,74,111,105,110,116,0,0,0,49,49,98,50,69,100,103,101,83,104,97,112,101,0,0,0,0,0,0,0,136,23,0,0,168,30,0,0,0,0,0,0,0,0,0,0,224,23,0,0,232,30,0,0,0,0,0,0,0,0,0,0,96,24,0,0,184,31,0,0,0,0,0,0,0,0,0,0,208,24,0,0,160,30,0,0,0,0,0,0,0,0,0,0,48,25,0,0,248,30,0,0,0,0,0,0,0,0,0,0,120,25,0,0,168,30,0,0,0,0,0,0,0,0,0,0,216,25,0,0,248,30,0,0,0,0,0,0,0,0,0,0,40,26,0,0,0,0,0,0,56,26,0,0,0,0,0,0,72,26,0,0,136,30,0,0,0,0,0,0,0,0,0,0,88,26,0,0,0,0,0,0,128,26,0,0,0,0,0,0,168,26,0,0,192,30,0,0,0,0,0,0,0,0,0,0,208,26,0,0,208,30,0,0,0,0,0,0,0,0,0,0,248,26,0,0,128,30,0,0,0,0,0,0,0,0,0,0,32,27,0,0,0,0,0,0,48,27,0,0,0,0,0,0,64,27,0,0,0,0,0,0,80,27,0,0,184,31,0,0,0,0,0,0,0,0,0,0,120,27,0,0,224,30,0,0,0,0,0,0,0,0,0,0,152,27,0,0,224,30,0,0,0,0,0,0,0,0,0,0,184,27,0,0,224,30,0,0,0,0,0,0,0,0,0,0,216,27,0,0,224,30,0,0,0,0,0,0,0,0,0,0,248,27,0,0,112,31,0,0,0,0,0,0,0,0,0,0,24,28,0,0,224,30,0,0,0,0,0,0,0,0,0,0,56,28,0,0,0,0,0,0,80,28,0,0,0,0,0,0,104,28,0,0,184,31,0,0,0,0,0,0,0,0,0,0,128,28,0,0,240,30,0,0,0,0,0,0,0,0,0,0,152,28,0,0,224,30,0,0,0,0,0,0,0,0,0,0,176,28,0,0,240,30,0,0,0,0,0,0,0,0,0,0,200,28,0,0,0,0,0,0,224,28,0,0,240,30,0,0,0,0,0,0,0,0,0,0,248,28,0,0,240,30,0,0,0,0,0,0,0,0,0,0,16,29,0,0,0,0,0,0,40,29,0,0,224,30,0,0,0,0,0,0,0,0,0,0,64,29,0,0,104,31,0,0,0,0,0,0,0,0,0,0,88,29,0,0,232,30,0,0,0,0,0,0,0,0,0,0,112,29,0,0,240,30,0,0,0,0,0,0,0,0,0,0,128,29,0,0,232,30,0,0,0,0,0,0,0,0,0,0,144,29,0,0,240,30,0,0,0,0,0,0,0,0,0,0,160,29,0,0,240,30,0,0,0,0,0,0,0,0,0,0,176,29,0,0,240,30,0,0,0,0,0,0,0,0,0,0,192,29,0,0,232,30,0,0,0,0,0,0,0,0,0,0,208,29,0,0,240,30,0,0,0,0,0,0,0,0,0,0,224,29,0,0,240,30,0,0,0,0,0,0,0,0,0,0,240,29,0,0,240,30,0,0,0,0,0,0,0,0,0,0,0,30,0,0,232,30,0,0,0,0,0,0,16,0,0,0,32,0,0,0,64,0,0,0,96,0,0,0,128,0,0,0,160,0,0,0,192,0,0,0,224,0,0,0,0,1,0,0,64,1,0,0,128,1,0,0,192,1,0,0,0,2,0,0,128,2,0,0,8,0,0,0,0,0,0,0,10,0,0,0,0,0,0,0,46,0,0,0,0,0,0,0], "i8", ALLOC_NONE, Runtime.GLOBAL_BASE);



var tempDoublePtr = Runtime.alignMemory(allocate(12, "i8", ALLOC_STATIC), 8);

assert(tempDoublePtr % 8 == 0);

function copyTempFloat(ptr) { // functions, because inlining this code increases code size too much

  HEAP8[tempDoublePtr] = HEAP8[ptr];

  HEAP8[tempDoublePtr+1] = HEAP8[ptr+1];

  HEAP8[tempDoublePtr+2] = HEAP8[ptr+2];

  HEAP8[tempDoublePtr+3] = HEAP8[ptr+3];

}

function copyTempDouble(ptr) {

  HEAP8[tempDoublePtr] = HEAP8[ptr];

  HEAP8[tempDoublePtr+1] = HEAP8[ptr+1];

  HEAP8[tempDoublePtr+2] = HEAP8[ptr+2];

  HEAP8[tempDoublePtr+3] = HEAP8[ptr+3];

  HEAP8[tempDoublePtr+4] = HEAP8[ptr+4];

  HEAP8[tempDoublePtr+5] = HEAP8[ptr+5];

  HEAP8[tempDoublePtr+6] = HEAP8[ptr+6];

  HEAP8[tempDoublePtr+7] = HEAP8[ptr+7];

}


  
  function __ZSt18uncaught_exceptionv() { // std::uncaught_exception()
      return !!__ZSt18uncaught_exceptionv.uncaught_exception;
    }
  
  
  
  function ___cxa_is_number_type(type) {
      var isNumber = false;
      try { if (type == __ZTIi) isNumber = true } catch(e){}
      try { if (type == __ZTIj) isNumber = true } catch(e){}
      try { if (type == __ZTIl) isNumber = true } catch(e){}
      try { if (type == __ZTIm) isNumber = true } catch(e){}
      try { if (type == __ZTIx) isNumber = true } catch(e){}
      try { if (type == __ZTIy) isNumber = true } catch(e){}
      try { if (type == __ZTIf) isNumber = true } catch(e){}
      try { if (type == __ZTId) isNumber = true } catch(e){}
      try { if (type == __ZTIe) isNumber = true } catch(e){}
      try { if (type == __ZTIc) isNumber = true } catch(e){}
      try { if (type == __ZTIa) isNumber = true } catch(e){}
      try { if (type == __ZTIh) isNumber = true } catch(e){}
      try { if (type == __ZTIs) isNumber = true } catch(e){}
      try { if (type == __ZTIt) isNumber = true } catch(e){}
      return isNumber;
    }function ___cxa_does_inherit(definiteType, possibilityType, possibility) {
      if (possibility == 0) return false;
      if (possibilityType == 0 || possibilityType == definiteType)
        return true;
      var possibility_type_info;
      if (___cxa_is_number_type(possibilityType)) {
        possibility_type_info = possibilityType;
      } else {
        var possibility_type_infoAddr = HEAP32[((possibilityType)>>2)] - 8;
        possibility_type_info = HEAP32[((possibility_type_infoAddr)>>2)];
      }
      switch (possibility_type_info) {
      case 0: // possibility is a pointer
        // See if definite type is a pointer
        var definite_type_infoAddr = HEAP32[((definiteType)>>2)] - 8;
        var definite_type_info = HEAP32[((definite_type_infoAddr)>>2)];
        if (definite_type_info == 0) {
          // Also a pointer; compare base types of pointers
          var defPointerBaseAddr = definiteType+8;
          var defPointerBaseType = HEAP32[((defPointerBaseAddr)>>2)];
          var possPointerBaseAddr = possibilityType+8;
          var possPointerBaseType = HEAP32[((possPointerBaseAddr)>>2)];
          return ___cxa_does_inherit(defPointerBaseType, possPointerBaseType, possibility);
        } else
          return false; // one pointer and one non-pointer
      case 1: // class with no base class
        return false;
      case 2: // class with base class
        var parentTypeAddr = possibilityType + 8;
        var parentType = HEAP32[((parentTypeAddr)>>2)];
        return ___cxa_does_inherit(definiteType, parentType, possibility);
      default:
        return false; // some unencountered type
      }
    }
  
  
  
  var ___cxa_last_thrown_exception=0;function ___resumeException(ptr) {
      if (!___cxa_last_thrown_exception) { ___cxa_last_thrown_exception = ptr; }
      throw ptr + " - Exception catching is disabled, this exception cannot be caught. Compile with -s DISABLE_EXCEPTION_CATCHING=0 or DISABLE_EXCEPTION_CATCHING=2 to catch.";
    }
  
  var ___cxa_exception_header_size=8;function ___cxa_find_matching_catch(thrown, throwntype) {
      if (thrown == -1) thrown = ___cxa_last_thrown_exception;
      header = thrown - ___cxa_exception_header_size;
      if (throwntype == -1) throwntype = HEAP32[((header)>>2)];
      var typeArray = Array.prototype.slice.call(arguments, 2);
  
      // If throwntype is a pointer, this means a pointer has been
      // thrown. When a pointer is thrown, actually what's thrown
      // is a pointer to the pointer. We'll dereference it.
      if (throwntype != 0 && !___cxa_is_number_type(throwntype)) {
        var throwntypeInfoAddr= HEAP32[((throwntype)>>2)] - 8;
        var throwntypeInfo= HEAP32[((throwntypeInfoAddr)>>2)];
        if (throwntypeInfo == 0)
          thrown = HEAP32[((thrown)>>2)];
      }
      // The different catch blocks are denoted by different types.
      // Due to inheritance, those types may not precisely match the
      // type of the thrown object. Find one which matches, and
      // return the type of the catch block which should be called.
      for (var i = 0; i < typeArray.length; i++) {
        if (___cxa_does_inherit(typeArray[i], throwntype, thrown))
          return ((asm["setTempRet0"](typeArray[i]),thrown)|0);
      }
      // Shouldn't happen unless we have bogus data in typeArray
      // or encounter a type for which emscripten doesn't have suitable
      // typeinfo defined. Best-efforts match just in case.
      return ((asm["setTempRet0"](throwntype),thrown)|0);
    }function ___gxx_personality_v0() {
    }

  
  
  function __exit(status) {
      // void _exit(int status);
      // http://pubs.opengroup.org/onlinepubs/000095399/functions/exit.html
      Module['exit'](status);
    }function _exit(status) {
      __exit(status);
    }function __ZSt9terminatev() {
      _exit(-1234);
    }

  function _b2WorldRayCastCallback(fixturePtr, pointX, pointY,
                                     normalX, normalY, fraction) {
      return b2World.RayCast(fixturePtr, pointX, pointY, normalX, normalY, fraction);
    }

  function _b2WorldQueryAABB(fixturePtr) {
      return b2World.QueryAABB(fixturePtr);
    }

  function _b2WorldPostSolve(contactPtr, impulsePtr) {
      b2World.PostSolve(contactPtr, impulsePtr);
    }

  function _b2WorldPreSolve(contactPtr, oldManifoldPtr) {
      b2World.PreSolve(contactPtr, oldManifoldPtr);
    }

  function _b2WorldEndContactBody(contactPtr) {
      b2World.EndContactBody(contactPtr);
    }

  function _b2WorldBeginContactBody(contactPtr) {
      b2World.BeginContactBody(contactPtr);
    }

  
   
  Module["_memset"] = _memset;var _llvm_memset_p0i8_i64=_memset;

  var _sqrtf=Math_sqrt;

  
  
  function _emscripten_memcpy_big(dest, src, num) {
      HEAPU8.set(HEAPU8.subarray(src, src+num), dest);
      return dest;
    } 
  Module["_memcpy"] = _memcpy;var _llvm_memcpy_p0i8_p0i8_i32=_memcpy;

  var _sinf=Math_sin;

  var _cosf=Math_cos;

  var _floorf=Math_floor;

  var _llvm_memset_p0i8_i32=_memset;

  function _llvm_lifetime_start() {}

  function _llvm_lifetime_end() {}

  function ___cxa_pure_virtual() {
      ABORT = true;
      throw 'Pure virtual function called!';
    }

  
   
  Module["_memmove"] = _memmove;var _llvm_memmove_p0i8_p0i8_i32=_memmove;

  function __ZNSt9exceptionD2Ev() {}

  function _abort() {
      Module['abort']();
    }

  
  
  var ___errno_state=0;function ___setErrNo(value) {
      // For convenient setting and returning of errno.
      HEAP32[((___errno_state)>>2)]=value;
      return value;
    }function ___errno_location() {
      return ___errno_state;
    }

  function _sbrk(bytes) {
      // Implement a Linux-like 'memory area' for our 'process'.
      // Changes the size of the memory area by |bytes|; returns the
      // address of the previous top ('break') of the memory area
      // We control the "dynamic" memory - DYNAMIC_BASE to DYNAMICTOP
      var self = _sbrk;
      if (!self.called) {
        DYNAMICTOP = alignMemoryPage(DYNAMICTOP); // make sure we start out aligned
        self.called = true;
        assert(Runtime.dynamicAlloc);
        self.alloc = Runtime.dynamicAlloc;
        Runtime.dynamicAlloc = function() { abort('cannot dynamically allocate, sbrk now has control') };
      }
      var ret = DYNAMICTOP;
      if (bytes != 0) self.alloc(bytes);
      return ret;  // Previous break location.
    }

  
  var ERRNO_CODES={EPERM:1,ENOENT:2,ESRCH:3,EINTR:4,EIO:5,ENXIO:6,E2BIG:7,ENOEXEC:8,EBADF:9,ECHILD:10,EAGAIN:11,EWOULDBLOCK:11,ENOMEM:12,EACCES:13,EFAULT:14,ENOTBLK:15,EBUSY:16,EEXIST:17,EXDEV:18,ENODEV:19,ENOTDIR:20,EISDIR:21,EINVAL:22,ENFILE:23,EMFILE:24,ENOTTY:25,ETXTBSY:26,EFBIG:27,ENOSPC:28,ESPIPE:29,EROFS:30,EMLINK:31,EPIPE:32,EDOM:33,ERANGE:34,ENOMSG:42,EIDRM:43,ECHRNG:44,EL2NSYNC:45,EL3HLT:46,EL3RST:47,ELNRNG:48,EUNATCH:49,ENOCSI:50,EL2HLT:51,EDEADLK:35,ENOLCK:37,EBADE:52,EBADR:53,EXFULL:54,ENOANO:55,EBADRQC:56,EBADSLT:57,EDEADLOCK:35,EBFONT:59,ENOSTR:60,ENODATA:61,ETIME:62,ENOSR:63,ENONET:64,ENOPKG:65,EREMOTE:66,ENOLINK:67,EADV:68,ESRMNT:69,ECOMM:70,EPROTO:71,EMULTIHOP:72,EDOTDOT:73,EBADMSG:74,ENOTUNIQ:76,EBADFD:77,EREMCHG:78,ELIBACC:79,ELIBBAD:80,ELIBSCN:81,ELIBMAX:82,ELIBEXEC:83,ENOSYS:38,ENOTEMPTY:39,ENAMETOOLONG:36,ELOOP:40,EOPNOTSUPP:95,EPFNOSUPPORT:96,ECONNRESET:104,ENOBUFS:105,EAFNOSUPPORT:97,EPROTOTYPE:91,ENOTSOCK:88,ENOPROTOOPT:92,ESHUTDOWN:108,ECONNREFUSED:111,EADDRINUSE:98,ECONNABORTED:103,ENETUNREACH:101,ENETDOWN:100,ETIMEDOUT:110,EHOSTDOWN:112,EHOSTUNREACH:113,EINPROGRESS:115,EALREADY:114,EDESTADDRREQ:89,EMSGSIZE:90,EPROTONOSUPPORT:93,ESOCKTNOSUPPORT:94,EADDRNOTAVAIL:99,ENETRESET:102,EISCONN:106,ENOTCONN:107,ETOOMANYREFS:109,EUSERS:87,EDQUOT:122,ESTALE:116,ENOTSUP:95,ENOMEDIUM:123,EILSEQ:84,EOVERFLOW:75,ECANCELED:125,ENOTRECOVERABLE:131,EOWNERDEAD:130,ESTRPIPE:86};function _sysconf(name) {
      // long sysconf(int name);
      // http://pubs.opengroup.org/onlinepubs/009695399/functions/sysconf.html
      switch(name) {
        case 30: return PAGE_SIZE;
        case 132:
        case 133:
        case 12:
        case 137:
        case 138:
        case 15:
        case 235:
        case 16:
        case 17:
        case 18:
        case 19:
        case 20:
        case 149:
        case 13:
        case 10:
        case 236:
        case 153:
        case 9:
        case 21:
        case 22:
        case 159:
        case 154:
        case 14:
        case 77:
        case 78:
        case 139:
        case 80:
        case 81:
        case 79:
        case 82:
        case 68:
        case 67:
        case 164:
        case 11:
        case 29:
        case 47:
        case 48:
        case 95:
        case 52:
        case 51:
        case 46:
          return 200809;
        case 27:
        case 246:
        case 127:
        case 128:
        case 23:
        case 24:
        case 160:
        case 161:
        case 181:
        case 182:
        case 242:
        case 183:
        case 184:
        case 243:
        case 244:
        case 245:
        case 165:
        case 178:
        case 179:
        case 49:
        case 50:
        case 168:
        case 169:
        case 175:
        case 170:
        case 171:
        case 172:
        case 97:
        case 76:
        case 32:
        case 173:
        case 35:
          return -1;
        case 176:
        case 177:
        case 7:
        case 155:
        case 8:
        case 157:
        case 125:
        case 126:
        case 92:
        case 93:
        case 129:
        case 130:
        case 131:
        case 94:
        case 91:
          return 1;
        case 74:
        case 60:
        case 69:
        case 70:
        case 4:
          return 1024;
        case 31:
        case 42:
        case 72:
          return 32;
        case 87:
        case 26:
        case 33:
          return 2147483647;
        case 34:
        case 1:
          return 47839;
        case 38:
        case 36:
          return 99;
        case 43:
        case 37:
          return 2048;
        case 0: return 2097152;
        case 3: return 65536;
        case 28: return 32768;
        case 44: return 32767;
        case 75: return 16384;
        case 39: return 1000;
        case 89: return 700;
        case 71: return 256;
        case 40: return 255;
        case 2: return 100;
        case 180: return 64;
        case 25: return 20;
        case 5: return 16;
        case 6: return 6;
        case 73: return 4;
        case 84: return 1;
      }
      ___setErrNo(ERRNO_CODES.EINVAL);
      return -1;
    }

  function _time(ptr) {
      var ret = Math.floor(Date.now()/1000);
      if (ptr) {
        HEAP32[((ptr)>>2)]=ret;
      }
      return ret;
    }

  function ___cxa_allocate_exception(size) {
      var ptr = _malloc(size + ___cxa_exception_header_size);
      return ptr + ___cxa_exception_header_size;
    }

  function ___cxa_throw(ptr, type, destructor) {
      if (!___cxa_throw.initialized) {
        try {
          HEAP32[((__ZTVN10__cxxabiv119__pointer_type_infoE)>>2)]=0; // Workaround for libcxxabi integration bug
        } catch(e){}
        try {
          HEAP32[((__ZTVN10__cxxabiv117__class_type_infoE)>>2)]=1; // Workaround for libcxxabi integration bug
        } catch(e){}
        try {
          HEAP32[((__ZTVN10__cxxabiv120__si_class_type_infoE)>>2)]=2; // Workaround for libcxxabi integration bug
        } catch(e){}
        ___cxa_throw.initialized = true;
      }
      var header = ptr - ___cxa_exception_header_size;
      HEAP32[((header)>>2)]=type;
      HEAP32[(((header)+(4))>>2)]=destructor;
      ___cxa_last_thrown_exception = ptr;
      if (!("uncaught_exception" in __ZSt18uncaught_exceptionv)) {
        __ZSt18uncaught_exceptionv.uncaught_exception = 1;
      } else {
        __ZSt18uncaught_exceptionv.uncaught_exception++;
      }
      throw ptr + " - Exception catching is disabled, this exception cannot be caught. Compile with -s DISABLE_EXCEPTION_CATCHING=0 or DISABLE_EXCEPTION_CATCHING=2 to catch.";
    }

  function ___cxa_call_unexpected(exception) {
      Module.printErr('Unexpected exception thrown, this is not properly supported - aborting');
      ABORT = true;
      throw exception;
    }

  
  var ___cxa_caught_exceptions=[];function ___cxa_begin_catch(ptr) {
      __ZSt18uncaught_exceptionv.uncaught_exception--;
      ___cxa_caught_exceptions.push(___cxa_last_thrown_exception);
      return ptr;
    }

  
  function ___cxa_free_exception(ptr) {
      try {
        return _free(ptr - ___cxa_exception_header_size);
      } catch(e) { // XXX FIXME
      }
    }function ___cxa_end_catch() {
      if (___cxa_end_catch.rethrown) {
        ___cxa_end_catch.rethrown = false;
        return;
      }
      // Clear state flag.
      asm['setThrew'](0);
      // Call destructor if one is registered then clear it.
      var ptr = ___cxa_caught_exceptions.pop();
      if (ptr) {
        header = ptr - ___cxa_exception_header_size;
        var destructor = HEAP32[(((header)+(4))>>2)];
        if (destructor) {
          Runtime.dynCall('vi', destructor, [ptr]);
          HEAP32[(((header)+(4))>>2)]=0;
        }
        ___cxa_free_exception(ptr);
        ___cxa_last_thrown_exception = 0;
      }
    }





   
  Module["_strlen"] = _strlen;

  
  
  
  var ERRNO_MESSAGES={0:"Success",1:"Not super-user",2:"No such file or directory",3:"No such process",4:"Interrupted system call",5:"I/O error",6:"No such device or address",7:"Arg list too long",8:"Exec format error",9:"Bad file number",10:"No children",11:"No more processes",12:"Not enough core",13:"Permission denied",14:"Bad address",15:"Block device required",16:"Mount device busy",17:"File exists",18:"Cross-device link",19:"No such device",20:"Not a directory",21:"Is a directory",22:"Invalid argument",23:"Too many open files in system",24:"Too many open files",25:"Not a typewriter",26:"Text file busy",27:"File too large",28:"No space left on device",29:"Illegal seek",30:"Read only file system",31:"Too many links",32:"Broken pipe",33:"Math arg out of domain of func",34:"Math result not representable",35:"File locking deadlock error",36:"File or path name too long",37:"No record locks available",38:"Function not implemented",39:"Directory not empty",40:"Too many symbolic links",42:"No message of desired type",43:"Identifier removed",44:"Channel number out of range",45:"Level 2 not synchronized",46:"Level 3 halted",47:"Level 3 reset",48:"Link number out of range",49:"Protocol driver not attached",50:"No CSI structure available",51:"Level 2 halted",52:"Invalid exchange",53:"Invalid request descriptor",54:"Exchange full",55:"No anode",56:"Invalid request code",57:"Invalid slot",59:"Bad font file fmt",60:"Device not a stream",61:"No data (for no delay io)",62:"Timer expired",63:"Out of streams resources",64:"Machine is not on the network",65:"Package not installed",66:"The object is remote",67:"The link has been severed",68:"Advertise error",69:"Srmount error",70:"Communication error on send",71:"Protocol error",72:"Multihop attempted",73:"Cross mount point (not really error)",74:"Trying to read unreadable message",75:"Value too large for defined data type",76:"Given log. name not unique",77:"f.d. invalid for this operation",78:"Remote address changed",79:"Can   access a needed shared lib",80:"Accessing a corrupted shared lib",81:".lib section in a.out corrupted",82:"Attempting to link in too many libs",83:"Attempting to exec a shared library",84:"Illegal byte sequence",86:"Streams pipe error",87:"Too many users",88:"Socket operation on non-socket",89:"Destination address required",90:"Message too long",91:"Protocol wrong type for socket",92:"Protocol not available",93:"Unknown protocol",94:"Socket type not supported",95:"Not supported",96:"Protocol family not supported",97:"Address family not supported by protocol family",98:"Address already in use",99:"Address not available",100:"Network interface is not configured",101:"Network is unreachable",102:"Connection reset by network",103:"Connection aborted",104:"Connection reset by peer",105:"No buffer space available",106:"Socket is already connected",107:"Socket is not connected",108:"Can't send after socket shutdown",109:"Too many references",110:"Connection timed out",111:"Connection refused",112:"Host is down",113:"Host is unreachable",114:"Socket already connected",115:"Connection already in progress",116:"Stale file handle",122:"Quota exceeded",123:"No medium (in tape drive)",125:"Operation canceled",130:"Previous owner died",131:"State not recoverable"};
  
  var TTY={ttys:[],init:function () {
        // https://github.com/kripken/emscripten/pull/1555
        // if (ENVIRONMENT_IS_NODE) {
        //   // currently, FS.init does not distinguish if process.stdin is a file or TTY
        //   // device, it always assumes it's a TTY device. because of this, we're forcing
        //   // process.stdin to UTF8 encoding to at least make stdin reading compatible
        //   // with text files until FS.init can be refactored.
        //   process['stdin']['setEncoding']('utf8');
        // }
      },shutdown:function () {
        // https://github.com/kripken/emscripten/pull/1555
        // if (ENVIRONMENT_IS_NODE) {
        //   // inolen: any idea as to why node -e 'process.stdin.read()' wouldn't exit immediately (with process.stdin being a tty)?
        //   // isaacs: because now it's reading from the stream, you've expressed interest in it, so that read() kicks off a _read() which creates a ReadReq operation
        //   // inolen: I thought read() in that case was a synchronous operation that just grabbed some amount of buffered data if it exists?
        //   // isaacs: it is. but it also triggers a _read() call, which calls readStart() on the handle
        //   // isaacs: do process.stdin.pause() and i'd think it'd probably close the pending call
        //   process['stdin']['pause']();
        // }
      },register:function (dev, ops) {
        TTY.ttys[dev] = { input: [], output: [], ops: ops };
        FS.registerDevice(dev, TTY.stream_ops);
      },stream_ops:{open:function (stream) {
          var tty = TTY.ttys[stream.node.rdev];
          if (!tty) {
            throw new FS.ErrnoError(ERRNO_CODES.ENODEV);
          }
          stream.tty = tty;
          stream.seekable = false;
        },close:function (stream) {
          // flush any pending line data
          if (stream.tty.output.length) {
            stream.tty.ops.put_char(stream.tty, 10);
          }
        },read:function (stream, buffer, offset, length, pos /* ignored */) {
          if (!stream.tty || !stream.tty.ops.get_char) {
            throw new FS.ErrnoError(ERRNO_CODES.ENXIO);
          }
          var bytesRead = 0;
          for (var i = 0; i < length; i++) {
            var result;
            try {
              result = stream.tty.ops.get_char(stream.tty);
            } catch (e) {
              throw new FS.ErrnoError(ERRNO_CODES.EIO);
            }
            if (result === undefined && bytesRead === 0) {
              throw new FS.ErrnoError(ERRNO_CODES.EAGAIN);
            }
            if (result === null || result === undefined) break;
            bytesRead++;
            buffer[offset+i] = result;
          }
          if (bytesRead) {
            stream.node.timestamp = Date.now();
          }
          return bytesRead;
        },write:function (stream, buffer, offset, length, pos) {
          if (!stream.tty || !stream.tty.ops.put_char) {
            throw new FS.ErrnoError(ERRNO_CODES.ENXIO);
          }
          for (var i = 0; i < length; i++) {
            try {
              stream.tty.ops.put_char(stream.tty, buffer[offset+i]);
            } catch (e) {
              throw new FS.ErrnoError(ERRNO_CODES.EIO);
            }
          }
          if (length) {
            stream.node.timestamp = Date.now();
          }
          return i;
        }},default_tty_ops:{get_char:function (tty) {
          if (!tty.input.length) {
            var result = null;
            if (ENVIRONMENT_IS_NODE) {
              result = process['stdin']['read']();
              if (!result) {
                if (process['stdin']['_readableState'] && process['stdin']['_readableState']['ended']) {
                  return null;  // EOF
                }
                return undefined;  // no data available
              }
            } else if (typeof window != 'undefined' &&
              typeof window.prompt == 'function') {
              // Browser.
              result = window.prompt('Input: ');  // returns null on cancel
              if (result !== null) {
                result += '\n';
              }
            } else if (typeof readline == 'function') {
              // Command line.
              result = readline();
              if (result !== null) {
                result += '\n';
              }
            }
            if (!result) {
              return null;
            }
            tty.input = intArrayFromString(result, true);
          }
          return tty.input.shift();
        },put_char:function (tty, val) {
          if (val === null || val === 10) {
            Module['print'](tty.output.join(''));
            tty.output = [];
          } else {
            tty.output.push(TTY.utf8.processCChar(val));
          }
        }},default_tty1_ops:{put_char:function (tty, val) {
          if (val === null || val === 10) {
            Module['printErr'](tty.output.join(''));
            tty.output = [];
          } else {
            tty.output.push(TTY.utf8.processCChar(val));
          }
        }}};
  
  var MEMFS={ops_table:null,CONTENT_OWNING:1,CONTENT_FLEXIBLE:2,CONTENT_FIXED:3,mount:function (mount) {
        return MEMFS.createNode(null, '/', 16384 | 511 /* 0777 */, 0);
      },createNode:function (parent, name, mode, dev) {
        if (FS.isBlkdev(mode) || FS.isFIFO(mode)) {
          // no supported
          throw new FS.ErrnoError(ERRNO_CODES.EPERM);
        }
        if (!MEMFS.ops_table) {
          MEMFS.ops_table = {
            dir: {
              node: {
                getattr: MEMFS.node_ops.getattr,
                setattr: MEMFS.node_ops.setattr,
                lookup: MEMFS.node_ops.lookup,
                mknod: MEMFS.node_ops.mknod,
                rename: MEMFS.node_ops.rename,
                unlink: MEMFS.node_ops.unlink,
                rmdir: MEMFS.node_ops.rmdir,
                readdir: MEMFS.node_ops.readdir,
                symlink: MEMFS.node_ops.symlink
              },
              stream: {
                llseek: MEMFS.stream_ops.llseek
              }
            },
            file: {
              node: {
                getattr: MEMFS.node_ops.getattr,
                setattr: MEMFS.node_ops.setattr
              },
              stream: {
                llseek: MEMFS.stream_ops.llseek,
                read: MEMFS.stream_ops.read,
                write: MEMFS.stream_ops.write,
                allocate: MEMFS.stream_ops.allocate,
                mmap: MEMFS.stream_ops.mmap
              }
            },
            link: {
              node: {
                getattr: MEMFS.node_ops.getattr,
                setattr: MEMFS.node_ops.setattr,
                readlink: MEMFS.node_ops.readlink
              },
              stream: {}
            },
            chrdev: {
              node: {
                getattr: MEMFS.node_ops.getattr,
                setattr: MEMFS.node_ops.setattr
              },
              stream: FS.chrdev_stream_ops
            },
          };
        }
        var node = FS.createNode(parent, name, mode, dev);
        if (FS.isDir(node.mode)) {
          node.node_ops = MEMFS.ops_table.dir.node;
          node.stream_ops = MEMFS.ops_table.dir.stream;
          node.contents = {};
        } else if (FS.isFile(node.mode)) {
          node.node_ops = MEMFS.ops_table.file.node;
          node.stream_ops = MEMFS.ops_table.file.stream;
          node.contents = [];
          node.contentMode = MEMFS.CONTENT_FLEXIBLE;
        } else if (FS.isLink(node.mode)) {
          node.node_ops = MEMFS.ops_table.link.node;
          node.stream_ops = MEMFS.ops_table.link.stream;
        } else if (FS.isChrdev(node.mode)) {
          node.node_ops = MEMFS.ops_table.chrdev.node;
          node.stream_ops = MEMFS.ops_table.chrdev.stream;
        }
        node.timestamp = Date.now();
        // add the new node to the parent
        if (parent) {
          parent.contents[name] = node;
        }
        return node;
      },ensureFlexible:function (node) {
        if (node.contentMode !== MEMFS.CONTENT_FLEXIBLE) {
          var contents = node.contents;
          node.contents = Array.prototype.slice.call(contents);
          node.contentMode = MEMFS.CONTENT_FLEXIBLE;
        }
      },node_ops:{getattr:function (node) {
          var attr = {};
          // device numbers reuse inode numbers.
          attr.dev = FS.isChrdev(node.mode) ? node.id : 1;
          attr.ino = node.id;
          attr.mode = node.mode;
          attr.nlink = 1;
          attr.uid = 0;
          attr.gid = 0;
          attr.rdev = node.rdev;
          if (FS.isDir(node.mode)) {
            attr.size = 4096;
          } else if (FS.isFile(node.mode)) {
            attr.size = node.contents.length;
          } else if (FS.isLink(node.mode)) {
            attr.size = node.link.length;
          } else {
            attr.size = 0;
          }
          attr.atime = new Date(node.timestamp);
          attr.mtime = new Date(node.timestamp);
          attr.ctime = new Date(node.timestamp);
          // NOTE: In our implementation, st_blocks = Math.ceil(st_size/st_blksize),
          //       but this is not required by the standard.
          attr.blksize = 4096;
          attr.blocks = Math.ceil(attr.size / attr.blksize);
          return attr;
        },setattr:function (node, attr) {
          if (attr.mode !== undefined) {
            node.mode = attr.mode;
          }
          if (attr.timestamp !== undefined) {
            node.timestamp = attr.timestamp;
          }
          if (attr.size !== undefined) {
            MEMFS.ensureFlexible(node);
            var contents = node.contents;
            if (attr.size < contents.length) contents.length = attr.size;
            else while (attr.size > contents.length) contents.push(0);
          }
        },lookup:function (parent, name) {
          throw FS.genericErrors[ERRNO_CODES.ENOENT];
        },mknod:function (parent, name, mode, dev) {
          return MEMFS.createNode(parent, name, mode, dev);
        },rename:function (old_node, new_dir, new_name) {
          // if we're overwriting a directory at new_name, make sure it's empty.
          if (FS.isDir(old_node.mode)) {
            var new_node;
            try {
              new_node = FS.lookupNode(new_dir, new_name);
            } catch (e) {
            }
            if (new_node) {
              for (var i in new_node.contents) {
                throw new FS.ErrnoError(ERRNO_CODES.ENOTEMPTY);
              }
            }
          }
          // do the internal rewiring
          delete old_node.parent.contents[old_node.name];
          old_node.name = new_name;
          new_dir.contents[new_name] = old_node;
          old_node.parent = new_dir;
        },unlink:function (parent, name) {
          delete parent.contents[name];
        },rmdir:function (parent, name) {
          var node = FS.lookupNode(parent, name);
          for (var i in node.contents) {
            throw new FS.ErrnoError(ERRNO_CODES.ENOTEMPTY);
          }
          delete parent.contents[name];
        },readdir:function (node) {
          var entries = ['.', '..']
          for (var key in node.contents) {
            if (!node.contents.hasOwnProperty(key)) {
              continue;
            }
            entries.push(key);
          }
          return entries;
        },symlink:function (parent, newname, oldpath) {
          var node = MEMFS.createNode(parent, newname, 511 /* 0777 */ | 40960, 0);
          node.link = oldpath;
          return node;
        },readlink:function (node) {
          if (!FS.isLink(node.mode)) {
            throw new FS.ErrnoError(ERRNO_CODES.EINVAL);
          }
          return node.link;
        }},stream_ops:{read:function (stream, buffer, offset, length, position) {
          var contents = stream.node.contents;
          if (position >= contents.length)
            return 0;
          var size = Math.min(contents.length - position, length);
          assert(size >= 0);
          if (size > 8 && contents.subarray) { // non-trivial, and typed array
            buffer.set(contents.subarray(position, position + size), offset);
          } else
          {
            for (var i = 0; i < size; i++) {
              buffer[offset + i] = contents[position + i];
            }
          }
          return size;
        },write:function (stream, buffer, offset, length, position, canOwn) {
          var node = stream.node;
          node.timestamp = Date.now();
          var contents = node.contents;
          if (length && contents.length === 0 && position === 0 && buffer.subarray) {
            // just replace it with the new data
            if (canOwn && offset === 0) {
              node.contents = buffer; // this could be a subarray of Emscripten HEAP, or allocated from some other source.
              node.contentMode = (buffer.buffer === HEAP8.buffer) ? MEMFS.CONTENT_OWNING : MEMFS.CONTENT_FIXED;
            } else {
              node.contents = new Uint8Array(buffer.subarray(offset, offset+length));
              node.contentMode = MEMFS.CONTENT_FIXED;
            }
            return length;
          }
          MEMFS.ensureFlexible(node);
          var contents = node.contents;
          while (contents.length < position) contents.push(0);
          for (var i = 0; i < length; i++) {
            contents[position + i] = buffer[offset + i];
          }
          return length;
        },llseek:function (stream, offset, whence) {
          var position = offset;
          if (whence === 1) {  // SEEK_CUR.
            position += stream.position;
          } else if (whence === 2) {  // SEEK_END.
            if (FS.isFile(stream.node.mode)) {
              position += stream.node.contents.length;
            }
          }
          if (position < 0) {
            throw new FS.ErrnoError(ERRNO_CODES.EINVAL);
          }
          stream.ungotten = [];
          stream.position = position;
          return position;
        },allocate:function (stream, offset, length) {
          MEMFS.ensureFlexible(stream.node);
          var contents = stream.node.contents;
          var limit = offset + length;
          while (limit > contents.length) contents.push(0);
        },mmap:function (stream, buffer, offset, length, position, prot, flags) {
          if (!FS.isFile(stream.node.mode)) {
            throw new FS.ErrnoError(ERRNO_CODES.ENODEV);
          }
          var ptr;
          var allocated;
          var contents = stream.node.contents;
          // Only make a new copy when MAP_PRIVATE is specified.
          if ( !(flags & 2) &&
                (contents.buffer === buffer || contents.buffer === buffer.buffer) ) {
            // We can't emulate MAP_SHARED when the file is not backed by the buffer
            // we're mapping to (e.g. the HEAP buffer).
            allocated = false;
            ptr = contents.byteOffset;
          } else {
            // Try to avoid unnecessary slices.
            if (position > 0 || position + length < contents.length) {
              if (contents.subarray) {
                contents = contents.subarray(position, position + length);
              } else {
                contents = Array.prototype.slice.call(contents, position, position + length);
              }
            }
            allocated = true;
            ptr = _malloc(length);
            if (!ptr) {
              throw new FS.ErrnoError(ERRNO_CODES.ENOMEM);
            }
            buffer.set(contents, ptr);
          }
          return { ptr: ptr, allocated: allocated };
        }}};
  
  var IDBFS={dbs:{},indexedDB:function () {
        return window.indexedDB || window.mozIndexedDB || window.webkitIndexedDB || window.msIndexedDB;
      },DB_VERSION:21,DB_STORE_NAME:"FILE_DATA",mount:function (mount) {
        // reuse all of the core MEMFS functionality
        return MEMFS.mount.apply(null, arguments);
      },syncfs:function (mount, populate, callback) {
        IDBFS.getLocalSet(mount, function(err, local) {
          if (err) return callback(err);
  
          IDBFS.getRemoteSet(mount, function(err, remote) {
            if (err) return callback(err);
  
            var src = populate ? remote : local;
            var dst = populate ? local : remote;
  
            IDBFS.reconcile(src, dst, callback);
          });
        });
      },getDB:function (name, callback) {
        // check the cache first
        var db = IDBFS.dbs[name];
        if (db) {
          return callback(null, db);
        }
  
        var req;
        try {
          req = IDBFS.indexedDB().open(name, IDBFS.DB_VERSION);
        } catch (e) {
          return callback(e);
        }
        req.onupgradeneeded = function(e) {
          var db = e.target.result;
          var transaction = e.target.transaction;
  
          var fileStore;
  
          if (db.objectStoreNames.contains(IDBFS.DB_STORE_NAME)) {
            fileStore = transaction.objectStore(IDBFS.DB_STORE_NAME);
          } else {
            fileStore = db.createObjectStore(IDBFS.DB_STORE_NAME);
          }
  
          fileStore.createIndex('timestamp', 'timestamp', { unique: false });
        };
        req.onsuccess = function() {
          db = req.result;
  
          // add to the cache
          IDBFS.dbs[name] = db;
          callback(null, db);
        };
        req.onerror = function() {
          callback(this.error);
        };
      },getLocalSet:function (mount, callback) {
        var entries = {};
  
        function isRealDir(p) {
          return p !== '.' && p !== '..';
        };
        function toAbsolute(root) {
          return function(p) {
            return PATH.join2(root, p);
          }
        };
  
        var check = FS.readdir(mount.mountpoint).filter(isRealDir).map(toAbsolute(mount.mountpoint));
  
        while (check.length) {
          var path = check.pop();
          var stat;
  
          try {
            stat = FS.stat(path);
          } catch (e) {
            return callback(e);
          }
  
          if (FS.isDir(stat.mode)) {
            check.push.apply(check, FS.readdir(path).filter(isRealDir).map(toAbsolute(path)));
          }
  
          entries[path] = { timestamp: stat.mtime };
        }
  
        return callback(null, { type: 'local', entries: entries });
      },getRemoteSet:function (mount, callback) {
        var entries = {};
  
        IDBFS.getDB(mount.mountpoint, function(err, db) {
          if (err) return callback(err);
  
          var transaction = db.transaction([IDBFS.DB_STORE_NAME], 'readonly');
          transaction.onerror = function() { callback(this.error); };
  
          var store = transaction.objectStore(IDBFS.DB_STORE_NAME);
          var index = store.index('timestamp');
  
          index.openKeyCursor().onsuccess = function(event) {
            var cursor = event.target.result;
  
            if (!cursor) {
              return callback(null, { type: 'remote', db: db, entries: entries });
            }
  
            entries[cursor.primaryKey] = { timestamp: cursor.key };
  
            cursor.continue();
          };
        });
      },loadLocalEntry:function (path, callback) {
        var stat, node;
  
        try {
          var lookup = FS.lookupPath(path);
          node = lookup.node;
          stat = FS.stat(path);
        } catch (e) {
          return callback(e);
        }
  
        if (FS.isDir(stat.mode)) {
          return callback(null, { timestamp: stat.mtime, mode: stat.mode });
        } else if (FS.isFile(stat.mode)) {
          return callback(null, { timestamp: stat.mtime, mode: stat.mode, contents: node.contents });
        } else {
          return callback(new Error('node type not supported'));
        }
      },storeLocalEntry:function (path, entry, callback) {
        try {
          if (FS.isDir(entry.mode)) {
            FS.mkdir(path, entry.mode);
          } else if (FS.isFile(entry.mode)) {
            FS.writeFile(path, entry.contents, { encoding: 'binary', canOwn: true });
          } else {
            return callback(new Error('node type not supported'));
          }
  
          FS.utime(path, entry.timestamp, entry.timestamp);
        } catch (e) {
          return callback(e);
        }
  
        callback(null);
      },removeLocalEntry:function (path, callback) {
        try {
          var lookup = FS.lookupPath(path);
          var stat = FS.stat(path);
  
          if (FS.isDir(stat.mode)) {
            FS.rmdir(path);
          } else if (FS.isFile(stat.mode)) {
            FS.unlink(path);
          }
        } catch (e) {
          return callback(e);
        }
  
        callback(null);
      },loadRemoteEntry:function (store, path, callback) {
        var req = store.get(path);
        req.onsuccess = function(event) { callback(null, event.target.result); };
        req.onerror = function() { callback(this.error); };
      },storeRemoteEntry:function (store, path, entry, callback) {
        var req = store.put(entry, path);
        req.onsuccess = function() { callback(null); };
        req.onerror = function() { callback(this.error); };
      },removeRemoteEntry:function (store, path, callback) {
        var req = store.delete(path);
        req.onsuccess = function() { callback(null); };
        req.onerror = function() { callback(this.error); };
      },reconcile:function (src, dst, callback) {
        var total = 0;
  
        var create = [];
        Object.keys(src.entries).forEach(function (key) {
          var e = src.entries[key];
          var e2 = dst.entries[key];
          if (!e2 || e.timestamp > e2.timestamp) {
            create.push(key);
            total++;
          }
        });
  
        var remove = [];
        Object.keys(dst.entries).forEach(function (key) {
          var e = dst.entries[key];
          var e2 = src.entries[key];
          if (!e2) {
            remove.push(key);
            total++;
          }
        });
  
        if (!total) {
          return callback(null);
        }
  
        var errored = false;
        var completed = 0;
        var db = src.type === 'remote' ? src.db : dst.db;
        var transaction = db.transaction([IDBFS.DB_STORE_NAME], 'readwrite');
        var store = transaction.objectStore(IDBFS.DB_STORE_NAME);
  
        function done(err) {
          if (err) {
            if (!done.errored) {
              done.errored = true;
              return callback(err);
            }
            return;
          }
          if (++completed >= total) {
            return callback(null);
          }
        };
  
        transaction.onerror = function() { done(this.error); };
  
        // sort paths in ascending order so directory entries are created
        // before the files inside them
        create.sort().forEach(function (path) {
          if (dst.type === 'local') {
            IDBFS.loadRemoteEntry(store, path, function (err, entry) {
              if (err) return done(err);
              IDBFS.storeLocalEntry(path, entry, done);
            });
          } else {
            IDBFS.loadLocalEntry(path, function (err, entry) {
              if (err) return done(err);
              IDBFS.storeRemoteEntry(store, path, entry, done);
            });
          }
        });
  
        // sort paths in descending order so files are deleted before their
        // parent directories
        remove.sort().reverse().forEach(function(path) {
          if (dst.type === 'local') {
            IDBFS.removeLocalEntry(path, done);
          } else {
            IDBFS.removeRemoteEntry(store, path, done);
          }
        });
      }};
  
  var NODEFS={isWindows:false,staticInit:function () {
        NODEFS.isWindows = !!process.platform.match(/^win/);
      },mount:function (mount) {
        assert(ENVIRONMENT_IS_NODE);
        return NODEFS.createNode(null, '/', NODEFS.getMode(mount.opts.root), 0);
      },createNode:function (parent, name, mode, dev) {
        if (!FS.isDir(mode) && !FS.isFile(mode) && !FS.isLink(mode)) {
          throw new FS.ErrnoError(ERRNO_CODES.EINVAL);
        }
        var node = FS.createNode(parent, name, mode);
        node.node_ops = NODEFS.node_ops;
        node.stream_ops = NODEFS.stream_ops;
        return node;
      },getMode:function (path) {
        var stat;
        try {
          stat = fs.lstatSync(path);
          if (NODEFS.isWindows) {
            // On Windows, directories return permission bits 'rw-rw-rw-', even though they have 'rwxrwxrwx', so 
            // propagate write bits to execute bits.
            stat.mode = stat.mode | ((stat.mode & 146) >> 1);
          }
        } catch (e) {
          if (!e.code) throw e;
          throw new FS.ErrnoError(ERRNO_CODES[e.code]);
        }
        return stat.mode;
      },realPath:function (node) {
        var parts = [];
        while (node.parent !== node) {
          parts.push(node.name);
          node = node.parent;
        }
        parts.push(node.mount.opts.root);
        parts.reverse();
        return PATH.join.apply(null, parts);
      },flagsToPermissionStringMap:{0:"r",1:"r+",2:"r+",64:"r",65:"r+",66:"r+",129:"rx+",193:"rx+",514:"w+",577:"w",578:"w+",705:"wx",706:"wx+",1024:"a",1025:"a",1026:"a+",1089:"a",1090:"a+",1153:"ax",1154:"ax+",1217:"ax",1218:"ax+",4096:"rs",4098:"rs+"},flagsToPermissionString:function (flags) {
        if (flags in NODEFS.flagsToPermissionStringMap) {
          return NODEFS.flagsToPermissionStringMap[flags];
        } else {
          return flags;
        }
      },node_ops:{getattr:function (node) {
          var path = NODEFS.realPath(node);
          var stat;
          try {
            stat = fs.lstatSync(path);
          } catch (e) {
            if (!e.code) throw e;
            throw new FS.ErrnoError(ERRNO_CODES[e.code]);
          }
          // node.js v0.10.20 doesn't report blksize and blocks on Windows. Fake them with default blksize of 4096.
          // See http://support.microsoft.com/kb/140365
          if (NODEFS.isWindows && !stat.blksize) {
            stat.blksize = 4096;
          }
          if (NODEFS.isWindows && !stat.blocks) {
            stat.blocks = (stat.size+stat.blksize-1)/stat.blksize|0;
          }
          return {
            dev: stat.dev,
            ino: stat.ino,
            mode: stat.mode,
            nlink: stat.nlink,
            uid: stat.uid,
            gid: stat.gid,
            rdev: stat.rdev,
            size: stat.size,
            atime: stat.atime,
            mtime: stat.mtime,
            ctime: stat.ctime,
            blksize: stat.blksize,
            blocks: stat.blocks
          };
        },setattr:function (node, attr) {
          var path = NODEFS.realPath(node);
          try {
            if (attr.mode !== undefined) {
              fs.chmodSync(path, attr.mode);
              // update the common node structure mode as well
              node.mode = attr.mode;
            }
            if (attr.timestamp !== undefined) {
              var date = new Date(attr.timestamp);
              fs.utimesSync(path, date, date);
            }
            if (attr.size !== undefined) {
              fs.truncateSync(path, attr.size);
            }
          } catch (e) {
            if (!e.code) throw e;
            throw new FS.ErrnoError(ERRNO_CODES[e.code]);
          }
        },lookup:function (parent, name) {
          var path = PATH.join2(NODEFS.realPath(parent), name);
          var mode = NODEFS.getMode(path);
          return NODEFS.createNode(parent, name, mode);
        },mknod:function (parent, name, mode, dev) {
          var node = NODEFS.createNode(parent, name, mode, dev);
          // create the backing node for this in the fs root as well
          var path = NODEFS.realPath(node);
          try {
            if (FS.isDir(node.mode)) {
              fs.mkdirSync(path, node.mode);
            } else {
              fs.writeFileSync(path, '', { mode: node.mode });
            }
          } catch (e) {
            if (!e.code) throw e;
            throw new FS.ErrnoError(ERRNO_CODES[e.code]);
          }
          return node;
        },rename:function (oldNode, newDir, newName) {
          var oldPath = NODEFS.realPath(oldNode);
          var newPath = PATH.join2(NODEFS.realPath(newDir), newName);
          try {
            fs.renameSync(oldPath, newPath);
          } catch (e) {
            if (!e.code) throw e;
            throw new FS.ErrnoError(ERRNO_CODES[e.code]);
          }
        },unlink:function (parent, name) {
          var path = PATH.join2(NODEFS.realPath(parent), name);
          try {
            fs.unlinkSync(path);
          } catch (e) {
            if (!e.code) throw e;
            throw new FS.ErrnoError(ERRNO_CODES[e.code]);
          }
        },rmdir:function (parent, name) {
          var path = PATH.join2(NODEFS.realPath(parent), name);
          try {
            fs.rmdirSync(path);
          } catch (e) {
            if (!e.code) throw e;
            throw new FS.ErrnoError(ERRNO_CODES[e.code]);
          }
        },readdir:function (node) {
          var path = NODEFS.realPath(node);
          try {
            return fs.readdirSync(path);
          } catch (e) {
            if (!e.code) throw e;
            throw new FS.ErrnoError(ERRNO_CODES[e.code]);
          }
        },symlink:function (parent, newName, oldPath) {
          var newPath = PATH.join2(NODEFS.realPath(parent), newName);
          try {
            fs.symlinkSync(oldPath, newPath);
          } catch (e) {
            if (!e.code) throw e;
            throw new FS.ErrnoError(ERRNO_CODES[e.code]);
          }
        },readlink:function (node) {
          var path = NODEFS.realPath(node);
          try {
            return fs.readlinkSync(path);
          } catch (e) {
            if (!e.code) throw e;
            throw new FS.ErrnoError(ERRNO_CODES[e.code]);
          }
        }},stream_ops:{open:function (stream) {
          var path = NODEFS.realPath(stream.node);
          try {
            if (FS.isFile(stream.node.mode)) {
              stream.nfd = fs.openSync(path, NODEFS.flagsToPermissionString(stream.flags));
            }
          } catch (e) {
            if (!e.code) throw e;
            throw new FS.ErrnoError(ERRNO_CODES[e.code]);
          }
        },close:function (stream) {
          try {
            if (FS.isFile(stream.node.mode) && stream.nfd) {
              fs.closeSync(stream.nfd);
            }
          } catch (e) {
            if (!e.code) throw e;
            throw new FS.ErrnoError(ERRNO_CODES[e.code]);
          }
        },read:function (stream, buffer, offset, length, position) {
          // FIXME this is terrible.
          var nbuffer = new Buffer(length);
          var res;
          try {
            res = fs.readSync(stream.nfd, nbuffer, 0, length, position);
          } catch (e) {
            throw new FS.ErrnoError(ERRNO_CODES[e.code]);
          }
          if (res > 0) {
            for (var i = 0; i < res; i++) {
              buffer[offset + i] = nbuffer[i];
            }
          }
          return res;
        },write:function (stream, buffer, offset, length, position) {
          // FIXME this is terrible.
          var nbuffer = new Buffer(buffer.subarray(offset, offset + length));
          var res;
          try {
            res = fs.writeSync(stream.nfd, nbuffer, 0, length, position);
          } catch (e) {
            throw new FS.ErrnoError(ERRNO_CODES[e.code]);
          }
          return res;
        },llseek:function (stream, offset, whence) {
          var position = offset;
          if (whence === 1) {  // SEEK_CUR.
            position += stream.position;
          } else if (whence === 2) {  // SEEK_END.
            if (FS.isFile(stream.node.mode)) {
              try {
                var stat = fs.fstatSync(stream.nfd);
                position += stat.size;
              } catch (e) {
                throw new FS.ErrnoError(ERRNO_CODES[e.code]);
              }
            }
          }
  
          if (position < 0) {
            throw new FS.ErrnoError(ERRNO_CODES.EINVAL);
          }
  
          stream.position = position;
          return position;
        }}};
  
  var _stdin=allocate(1, "i32*", ALLOC_STATIC);
  
  var _stdout=allocate(1, "i32*", ALLOC_STATIC);
  
  var _stderr=allocate(1, "i32*", ALLOC_STATIC);
  
  function _fflush(stream) {
      // int fflush(FILE *stream);
      // http://pubs.opengroup.org/onlinepubs/000095399/functions/fflush.html
      // we don't currently perform any user-space buffering of data
    }var FS={root:null,mounts:[],devices:[null],streams:[],nextInode:1,nameTable:null,currentPath:"/",initialized:false,ignorePermissions:true,ErrnoError:null,genericErrors:{},handleFSError:function (e) {
        if (!(e instanceof FS.ErrnoError)) throw e + ' : ' + stackTrace();
        return ___setErrNo(e.errno);
      },lookupPath:function (path, opts) {
        path = PATH.resolve(FS.cwd(), path);
        opts = opts || {};
  
        var defaults = {
          follow_mount: true,
          recurse_count: 0
        };
        for (var key in defaults) {
          if (opts[key] === undefined) {
            opts[key] = defaults[key];
          }
        }
  
        if (opts.recurse_count > 8) {  // max recursive lookup of 8
          throw new FS.ErrnoError(ERRNO_CODES.ELOOP);
        }
  
        // split the path
        var parts = PATH.normalizeArray(path.split('/').filter(function(p) {
          return !!p;
        }), false);
  
        // start at the root
        var current = FS.root;
        var current_path = '/';
  
        for (var i = 0; i < parts.length; i++) {
          var islast = (i === parts.length-1);
          if (islast && opts.parent) {
            // stop resolving
            break;
          }
  
          current = FS.lookupNode(current, parts[i]);
          current_path = PATH.join2(current_path, parts[i]);
  
          // jump to the mount's root node if this is a mountpoint
          if (FS.isMountpoint(current)) {
            if (!islast || (islast && opts.follow_mount)) {
              current = current.mounted.root;
            }
          }
  
          // by default, lookupPath will not follow a symlink if it is the final path component.
          // setting opts.follow = true will override this behavior.
          if (!islast || opts.follow) {
            var count = 0;
            while (FS.isLink(current.mode)) {
              var link = FS.readlink(current_path);
              current_path = PATH.resolve(PATH.dirname(current_path), link);
              
              var lookup = FS.lookupPath(current_path, { recurse_count: opts.recurse_count });
              current = lookup.node;
  
              if (count++ > 40) {  // limit max consecutive symlinks to 40 (SYMLOOP_MAX).
                throw new FS.ErrnoError(ERRNO_CODES.ELOOP);
              }
            }
          }
        }
  
        return { path: current_path, node: current };
      },getPath:function (node) {
        var path;
        while (true) {
          if (FS.isRoot(node)) {
            var mount = node.mount.mountpoint;
            if (!path) return mount;
            return mount[mount.length-1] !== '/' ? mount + '/' + path : mount + path;
          }
          path = path ? node.name + '/' + path : node.name;
          node = node.parent;
        }
      },hashName:function (parentid, name) {
        var hash = 0;
  
  
        for (var i = 0; i < name.length; i++) {
          hash = ((hash << 5) - hash + name.charCodeAt(i)) | 0;
        }
        return ((parentid + hash) >>> 0) % FS.nameTable.length;
      },hashAddNode:function (node) {
        var hash = FS.hashName(node.parent.id, node.name);
        node.name_next = FS.nameTable[hash];
        FS.nameTable[hash] = node;
      },hashRemoveNode:function (node) {
        var hash = FS.hashName(node.parent.id, node.name);
        if (FS.nameTable[hash] === node) {
          FS.nameTable[hash] = node.name_next;
        } else {
          var current = FS.nameTable[hash];
          while (current) {
            if (current.name_next === node) {
              current.name_next = node.name_next;
              break;
            }
            current = current.name_next;
          }
        }
      },lookupNode:function (parent, name) {
        var err = FS.mayLookup(parent);
        if (err) {
          throw new FS.ErrnoError(err);
        }
        var hash = FS.hashName(parent.id, name);
        for (var node = FS.nameTable[hash]; node; node = node.name_next) {
          var nodeName = node.name;
          if (node.parent.id === parent.id && nodeName === name) {
            return node;
          }
        }
        // if we failed to find it in the cache, call into the VFS
        return FS.lookup(parent, name);
      },createNode:function (parent, name, mode, rdev) {
        if (!FS.FSNode) {
          FS.FSNode = function(parent, name, mode, rdev) {
            if (!parent) {
              parent = this;  // root node sets parent to itself
            }
            this.parent = parent;
            this.mount = parent.mount;
            this.mounted = null;
            this.id = FS.nextInode++;
            this.name = name;
            this.mode = mode;
            this.node_ops = {};
            this.stream_ops = {};
            this.rdev = rdev;
          };
  
          FS.FSNode.prototype = {};
  
          // compatibility
          var readMode = 292 | 73;
          var writeMode = 146;
  
          // NOTE we must use Object.defineProperties instead of individual calls to
          // Object.defineProperty in order to make closure compiler happy
          Object.defineProperties(FS.FSNode.prototype, {
            read: {
              get: function() { return (this.mode & readMode) === readMode; },
              set: function(val) { val ? this.mode |= readMode : this.mode &= ~readMode; }
            },
            write: {
              get: function() { return (this.mode & writeMode) === writeMode; },
              set: function(val) { val ? this.mode |= writeMode : this.mode &= ~writeMode; }
            },
            isFolder: {
              get: function() { return FS.isDir(this.mode); },
            },
            isDevice: {
              get: function() { return FS.isChrdev(this.mode); },
            },
          });
        }
  
        var node = new FS.FSNode(parent, name, mode, rdev);
  
        FS.hashAddNode(node);
  
        return node;
      },destroyNode:function (node) {
        FS.hashRemoveNode(node);
      },isRoot:function (node) {
        return node === node.parent;
      },isMountpoint:function (node) {
        return !!node.mounted;
      },isFile:function (mode) {
        return (mode & 61440) === 32768;
      },isDir:function (mode) {
        return (mode & 61440) === 16384;
      },isLink:function (mode) {
        return (mode & 61440) === 40960;
      },isChrdev:function (mode) {
        return (mode & 61440) === 8192;
      },isBlkdev:function (mode) {
        return (mode & 61440) === 24576;
      },isFIFO:function (mode) {
        return (mode & 61440) === 4096;
      },isSocket:function (mode) {
        return (mode & 49152) === 49152;
      },flagModes:{"r":0,"rs":1052672,"r+":2,"w":577,"wx":705,"xw":705,"w+":578,"wx+":706,"xw+":706,"a":1089,"ax":1217,"xa":1217,"a+":1090,"ax+":1218,"xa+":1218},modeStringToFlags:function (str) {
        var flags = FS.flagModes[str];
        if (typeof flags === 'undefined') {
          throw new Error('Unknown file open mode: ' + str);
        }
        return flags;
      },flagsToPermissionString:function (flag) {
        var accmode = flag & 2097155;
        var perms = ['r', 'w', 'rw'][accmode];
        if ((flag & 512)) {
          perms += 'w';
        }
        return perms;
      },nodePermissions:function (node, perms) {
        if (FS.ignorePermissions) {
          return 0;
        }
        // return 0 if any user, group or owner bits are set.
        if (perms.indexOf('r') !== -1 && !(node.mode & 292)) {
          return ERRNO_CODES.EACCES;
        } else if (perms.indexOf('w') !== -1 && !(node.mode & 146)) {
          return ERRNO_CODES.EACCES;
        } else if (perms.indexOf('x') !== -1 && !(node.mode & 73)) {
          return ERRNO_CODES.EACCES;
        }
        return 0;
      },mayLookup:function (dir) {
        return FS.nodePermissions(dir, 'x');
      },mayCreate:function (dir, name) {
        try {
          var node = FS.lookupNode(dir, name);
          return ERRNO_CODES.EEXIST;
        } catch (e) {
        }
        return FS.nodePermissions(dir, 'wx');
      },mayDelete:function (dir, name, isdir) {
        var node;
        try {
          node = FS.lookupNode(dir, name);
        } catch (e) {
          return e.errno;
        }
        var err = FS.nodePermissions(dir, 'wx');
        if (err) {
          return err;
        }
        if (isdir) {
          if (!FS.isDir(node.mode)) {
            return ERRNO_CODES.ENOTDIR;
          }
          if (FS.isRoot(node) || FS.getPath(node) === FS.cwd()) {
            return ERRNO_CODES.EBUSY;
          }
        } else {
          if (FS.isDir(node.mode)) {
            return ERRNO_CODES.EISDIR;
          }
        }
        return 0;
      },mayOpen:function (node, flags) {
        if (!node) {
          return ERRNO_CODES.ENOENT;
        }
        if (FS.isLink(node.mode)) {
          return ERRNO_CODES.ELOOP;
        } else if (FS.isDir(node.mode)) {
          if ((flags & 2097155) !== 0 ||  // opening for write
              (flags & 512)) {
            return ERRNO_CODES.EISDIR;
          }
        }
        return FS.nodePermissions(node, FS.flagsToPermissionString(flags));
      },MAX_OPEN_FDS:4096,nextfd:function (fd_start, fd_end) {
        fd_start = fd_start || 0;
        fd_end = fd_end || FS.MAX_OPEN_FDS;
        for (var fd = fd_start; fd <= fd_end; fd++) {
          if (!FS.streams[fd]) {
            return fd;
          }
        }
        throw new FS.ErrnoError(ERRNO_CODES.EMFILE);
      },getStream:function (fd) {
        return FS.streams[fd];
      },createStream:function (stream, fd_start, fd_end) {
        if (!FS.FSStream) {
          FS.FSStream = function(){};
          FS.FSStream.prototype = {};
          // compatibility
          Object.defineProperties(FS.FSStream.prototype, {
            object: {
              get: function() { return this.node; },
              set: function(val) { this.node = val; }
            },
            isRead: {
              get: function() { return (this.flags & 2097155) !== 1; }
            },
            isWrite: {
              get: function() { return (this.flags & 2097155) !== 0; }
            },
            isAppend: {
              get: function() { return (this.flags & 1024); }
            }
          });
        }
        if (stream.__proto__) {
          // reuse the object
          stream.__proto__ = FS.FSStream.prototype;
        } else {
          var newStream = new FS.FSStream();
          for (var p in stream) {
            newStream[p] = stream[p];
          }
          stream = newStream;
        }
        var fd = FS.nextfd(fd_start, fd_end);
        stream.fd = fd;
        FS.streams[fd] = stream;
        return stream;
      },closeStream:function (fd) {
        FS.streams[fd] = null;
      },getStreamFromPtr:function (ptr) {
        return FS.streams[ptr - 1];
      },getPtrForStream:function (stream) {
        return stream ? stream.fd + 1 : 0;
      },chrdev_stream_ops:{open:function (stream) {
          var device = FS.getDevice(stream.node.rdev);
          // override node's stream ops with the device's
          stream.stream_ops = device.stream_ops;
          // forward the open call
          if (stream.stream_ops.open) {
            stream.stream_ops.open(stream);
          }
        },llseek:function () {
          throw new FS.ErrnoError(ERRNO_CODES.ESPIPE);
        }},major:function (dev) {
        return ((dev) >> 8);
      },minor:function (dev) {
        return ((dev) & 0xff);
      },makedev:function (ma, mi) {
        return ((ma) << 8 | (mi));
      },registerDevice:function (dev, ops) {
        FS.devices[dev] = { stream_ops: ops };
      },getDevice:function (dev) {
        return FS.devices[dev];
      },getMounts:function (mount) {
        var mounts = [];
        var check = [mount];
  
        while (check.length) {
          var m = check.pop();
  
          mounts.push(m);
  
          check.push.apply(check, m.mounts);
        }
  
        return mounts;
      },syncfs:function (populate, callback) {
        if (typeof(populate) === 'function') {
          callback = populate;
          populate = false;
        }
  
        var mounts = FS.getMounts(FS.root.mount);
        var completed = 0;
  
        function done(err) {
          if (err) {
            if (!done.errored) {
              done.errored = true;
              return callback(err);
            }
            return;
          }
          if (++completed >= mounts.length) {
            callback(null);
          }
        };
  
        // sync all mounts
        mounts.forEach(function (mount) {
          if (!mount.type.syncfs) {
            return done(null);
          }
          mount.type.syncfs(mount, populate, done);
        });
      },mount:function (type, opts, mountpoint) {
        var root = mountpoint === '/';
        var pseudo = !mountpoint;
        var node;
  
        if (root && FS.root) {
          throw new FS.ErrnoError(ERRNO_CODES.EBUSY);
        } else if (!root && !pseudo) {
          var lookup = FS.lookupPath(mountpoint, { follow_mount: false });
  
          mountpoint = lookup.path;  // use the absolute path
          node = lookup.node;
  
          if (FS.isMountpoint(node)) {
            throw new FS.ErrnoError(ERRNO_CODES.EBUSY);
          }
  
          if (!FS.isDir(node.mode)) {
            throw new FS.ErrnoError(ERRNO_CODES.ENOTDIR);
          }
        }
  
        var mount = {
          type: type,
          opts: opts,
          mountpoint: mountpoint,
          mounts: []
        };
  
        // create a root node for the fs
        var mountRoot = type.mount(mount);
        mountRoot.mount = mount;
        mount.root = mountRoot;
  
        if (root) {
          FS.root = mountRoot;
        } else if (node) {
          // set as a mountpoint
          node.mounted = mount;
  
          // add the new mount to the current mount's children
          if (node.mount) {
            node.mount.mounts.push(mount);
          }
        }
  
        return mountRoot;
      },unmount:function (mountpoint) {
        var lookup = FS.lookupPath(mountpoint, { follow_mount: false });
  
        if (!FS.isMountpoint(lookup.node)) {
          throw new FS.ErrnoError(ERRNO_CODES.EINVAL);
        }
  
        // destroy the nodes for this mount, and all its child mounts
        var node = lookup.node;
        var mount = node.mounted;
        var mounts = FS.getMounts(mount);
  
        Object.keys(FS.nameTable).forEach(function (hash) {
          var current = FS.nameTable[hash];
  
          while (current) {
            var next = current.name_next;
  
            if (mounts.indexOf(current.mount) !== -1) {
              FS.destroyNode(current);
            }
  
            current = next;
          }
        });
  
        // no longer a mountpoint
        node.mounted = null;
  
        // remove this mount from the child mounts
        var idx = node.mount.mounts.indexOf(mount);
        assert(idx !== -1);
        node.mount.mounts.splice(idx, 1);
      },lookup:function (parent, name) {
        return parent.node_ops.lookup(parent, name);
      },mknod:function (path, mode, dev) {
        var lookup = FS.lookupPath(path, { parent: true });
        var parent = lookup.node;
        var name = PATH.basename(path);
        var err = FS.mayCreate(parent, name);
        if (err) {
          throw new FS.ErrnoError(err);
        }
        if (!parent.node_ops.mknod) {
          throw new FS.ErrnoError(ERRNO_CODES.EPERM);
        }
        return parent.node_ops.mknod(parent, name, mode, dev);
      },create:function (path, mode) {
        mode = mode !== undefined ? mode : 438 /* 0666 */;
        mode &= 4095;
        mode |= 32768;
        return FS.mknod(path, mode, 0);
      },mkdir:function (path, mode) {
        mode = mode !== undefined ? mode : 511 /* 0777 */;
        mode &= 511 | 512;
        mode |= 16384;
        return FS.mknod(path, mode, 0);
      },mkdev:function (path, mode, dev) {
        if (typeof(dev) === 'undefined') {
          dev = mode;
          mode = 438 /* 0666 */;
        }
        mode |= 8192;
        return FS.mknod(path, mode, dev);
      },symlink:function (oldpath, newpath) {
        var lookup = FS.lookupPath(newpath, { parent: true });
        var parent = lookup.node;
        var newname = PATH.basename(newpath);
        var err = FS.mayCreate(parent, newname);
        if (err) {
          throw new FS.ErrnoError(err);
        }
        if (!parent.node_ops.symlink) {
          throw new FS.ErrnoError(ERRNO_CODES.EPERM);
        }
        return parent.node_ops.symlink(parent, newname, oldpath);
      },rename:function (old_path, new_path) {
        var old_dirname = PATH.dirname(old_path);
        var new_dirname = PATH.dirname(new_path);
        var old_name = PATH.basename(old_path);
        var new_name = PATH.basename(new_path);
        // parents must exist
        var lookup, old_dir, new_dir;
        try {
          lookup = FS.lookupPath(old_path, { parent: true });
          old_dir = lookup.node;
          lookup = FS.lookupPath(new_path, { parent: true });
          new_dir = lookup.node;
        } catch (e) {
          throw new FS.ErrnoError(ERRNO_CODES.EBUSY);
        }
        // need to be part of the same mount
        if (old_dir.mount !== new_dir.mount) {
          throw new FS.ErrnoError(ERRNO_CODES.EXDEV);
        }
        // source must exist
        var old_node = FS.lookupNode(old_dir, old_name);
        // old path should not be an ancestor of the new path
        var relative = PATH.relative(old_path, new_dirname);
        if (relative.charAt(0) !== '.') {
          throw new FS.ErrnoError(ERRNO_CODES.EINVAL);
        }
        // new path should not be an ancestor of the old path
        relative = PATH.relative(new_path, old_dirname);
        if (relative.charAt(0) !== '.') {
          throw new FS.ErrnoError(ERRNO_CODES.ENOTEMPTY);
        }
        // see if the new path already exists
        var new_node;
        try {
          new_node = FS.lookupNode(new_dir, new_name);
        } catch (e) {
          // not fatal
        }
        // early out if nothing needs to change
        if (old_node === new_node) {
          return;
        }
        // we'll need to delete the old entry
        var isdir = FS.isDir(old_node.mode);
        var err = FS.mayDelete(old_dir, old_name, isdir);
        if (err) {
          throw new FS.ErrnoError(err);
        }
        // need delete permissions if we'll be overwriting.
        // need create permissions if new doesn't already exist.
        err = new_node ?
          FS.mayDelete(new_dir, new_name, isdir) :
          FS.mayCreate(new_dir, new_name);
        if (err) {
          throw new FS.ErrnoError(err);
        }
        if (!old_dir.node_ops.rename) {
          throw new FS.ErrnoError(ERRNO_CODES.EPERM);
        }
        if (FS.isMountpoint(old_node) || (new_node && FS.isMountpoint(new_node))) {
          throw new FS.ErrnoError(ERRNO_CODES.EBUSY);
        }
        // if we are going to change the parent, check write permissions
        if (new_dir !== old_dir) {
          err = FS.nodePermissions(old_dir, 'w');
          if (err) {
            throw new FS.ErrnoError(err);
          }
        }
        // remove the node from the lookup hash
        FS.hashRemoveNode(old_node);
        // do the underlying fs rename
        try {
          old_dir.node_ops.rename(old_node, new_dir, new_name);
        } catch (e) {
          throw e;
        } finally {
          // add the node back to the hash (in case node_ops.rename
          // changed its name)
          FS.hashAddNode(old_node);
        }
      },rmdir:function (path) {
        var lookup = FS.lookupPath(path, { parent: true });
        var parent = lookup.node;
        var name = PATH.basename(path);
        var node = FS.lookupNode(parent, name);
        var err = FS.mayDelete(parent, name, true);
        if (err) {
          throw new FS.ErrnoError(err);
        }
        if (!parent.node_ops.rmdir) {
          throw new FS.ErrnoError(ERRNO_CODES.EPERM);
        }
        if (FS.isMountpoint(node)) {
          throw new FS.ErrnoError(ERRNO_CODES.EBUSY);
        }
        parent.node_ops.rmdir(parent, name);
        FS.destroyNode(node);
      },readdir:function (path) {
        var lookup = FS.lookupPath(path, { follow: true });
        var node = lookup.node;
        if (!node.node_ops.readdir) {
          throw new FS.ErrnoError(ERRNO_CODES.ENOTDIR);
        }
        return node.node_ops.readdir(node);
      },unlink:function (path) {
        var lookup = FS.lookupPath(path, { parent: true });
        var parent = lookup.node;
        var name = PATH.basename(path);
        var node = FS.lookupNode(parent, name);
        var err = FS.mayDelete(parent, name, false);
        if (err) {
          // POSIX says unlink should set EPERM, not EISDIR
          if (err === ERRNO_CODES.EISDIR) err = ERRNO_CODES.EPERM;
          throw new FS.ErrnoError(err);
        }
        if (!parent.node_ops.unlink) {
          throw new FS.ErrnoError(ERRNO_CODES.EPERM);
        }
        if (FS.isMountpoint(node)) {
          throw new FS.ErrnoError(ERRNO_CODES.EBUSY);
        }
        parent.node_ops.unlink(parent, name);
        FS.destroyNode(node);
      },readlink:function (path) {
        var lookup = FS.lookupPath(path);
        var link = lookup.node;
        if (!link.node_ops.readlink) {
          throw new FS.ErrnoError(ERRNO_CODES.EINVAL);
        }
        return link.node_ops.readlink(link);
      },stat:function (path, dontFollow) {
        var lookup = FS.lookupPath(path, { follow: !dontFollow });
        var node = lookup.node;
        if (!node.node_ops.getattr) {
          throw new FS.ErrnoError(ERRNO_CODES.EPERM);
        }
        return node.node_ops.getattr(node);
      },lstat:function (path) {
        return FS.stat(path, true);
      },chmod:function (path, mode, dontFollow) {
        var node;
        if (typeof path === 'string') {
          var lookup = FS.lookupPath(path, { follow: !dontFollow });
          node = lookup.node;
        } else {
          node = path;
        }
        if (!node.node_ops.setattr) {
          throw new FS.ErrnoError(ERRNO_CODES.EPERM);
        }
        node.node_ops.setattr(node, {
          mode: (mode & 4095) | (node.mode & ~4095),
          timestamp: Date.now()
        });
      },lchmod:function (path, mode) {
        FS.chmod(path, mode, true);
      },fchmod:function (fd, mode) {
        var stream = FS.getStream(fd);
        if (!stream) {
          throw new FS.ErrnoError(ERRNO_CODES.EBADF);
        }
        FS.chmod(stream.node, mode);
      },chown:function (path, uid, gid, dontFollow) {
        var node;
        if (typeof path === 'string') {
          var lookup = FS.lookupPath(path, { follow: !dontFollow });
          node = lookup.node;
        } else {
          node = path;
        }
        if (!node.node_ops.setattr) {
          throw new FS.ErrnoError(ERRNO_CODES.EPERM);
        }
        node.node_ops.setattr(node, {
          timestamp: Date.now()
          // we ignore the uid / gid for now
        });
      },lchown:function (path, uid, gid) {
        FS.chown(path, uid, gid, true);
      },fchown:function (fd, uid, gid) {
        var stream = FS.getStream(fd);
        if (!stream) {
          throw new FS.ErrnoError(ERRNO_CODES.EBADF);
        }
        FS.chown(stream.node, uid, gid);
      },truncate:function (path, len) {
        if (len < 0) {
          throw new FS.ErrnoError(ERRNO_CODES.EINVAL);
        }
        var node;
        if (typeof path === 'string') {
          var lookup = FS.lookupPath(path, { follow: true });
          node = lookup.node;
        } else {
          node = path;
        }
        if (!node.node_ops.setattr) {
          throw new FS.ErrnoError(ERRNO_CODES.EPERM);
        }
        if (FS.isDir(node.mode)) {
          throw new FS.ErrnoError(ERRNO_CODES.EISDIR);
        }
        if (!FS.isFile(node.mode)) {
          throw new FS.ErrnoError(ERRNO_CODES.EINVAL);
        }
        var err = FS.nodePermissions(node, 'w');
        if (err) {
          throw new FS.ErrnoError(err);
        }
        node.node_ops.setattr(node, {
          size: len,
          timestamp: Date.now()
        });
      },ftruncate:function (fd, len) {
        var stream = FS.getStream(fd);
        if (!stream) {
          throw new FS.ErrnoError(ERRNO_CODES.EBADF);
        }
        if ((stream.flags & 2097155) === 0) {
          throw new FS.ErrnoError(ERRNO_CODES.EINVAL);
        }
        FS.truncate(stream.node, len);
      },utime:function (path, atime, mtime) {
        var lookup = FS.lookupPath(path, { follow: true });
        var node = lookup.node;
        node.node_ops.setattr(node, {
          timestamp: Math.max(atime, mtime)
        });
      },open:function (path, flags, mode, fd_start, fd_end) {
        flags = typeof flags === 'string' ? FS.modeStringToFlags(flags) : flags;
        mode = typeof mode === 'undefined' ? 438 /* 0666 */ : mode;
        if ((flags & 64)) {
          mode = (mode & 4095) | 32768;
        } else {
          mode = 0;
        }
        var node;
        if (typeof path === 'object') {
          node = path;
        } else {
          path = PATH.normalize(path);
          try {
            var lookup = FS.lookupPath(path, {
              follow: !(flags & 131072)
            });
            node = lookup.node;
          } catch (e) {
            // ignore
          }
        }
        // perhaps we need to create the node
        if ((flags & 64)) {
          if (node) {
            // if O_CREAT and O_EXCL are set, error out if the node already exists
            if ((flags & 128)) {
              throw new FS.ErrnoError(ERRNO_CODES.EEXIST);
            }
          } else {
            // node doesn't exist, try to create it
            node = FS.mknod(path, mode, 0);
          }
        }
        if (!node) {
          throw new FS.ErrnoError(ERRNO_CODES.ENOENT);
        }
        // can't truncate a device
        if (FS.isChrdev(node.mode)) {
          flags &= ~512;
        }
        // check permissions
        var err = FS.mayOpen(node, flags);
        if (err) {
          throw new FS.ErrnoError(err);
        }
        // do truncation if necessary
        if ((flags & 512)) {
          FS.truncate(node, 0);
        }
        // we've already handled these, don't pass down to the underlying vfs
        flags &= ~(128 | 512);
  
        // register the stream with the filesystem
        var stream = FS.createStream({
          node: node,
          path: FS.getPath(node),  // we want the absolute path to the node
          flags: flags,
          seekable: true,
          position: 0,
          stream_ops: node.stream_ops,
          // used by the file family libc calls (fopen, fwrite, ferror, etc.)
          ungotten: [],
          error: false
        }, fd_start, fd_end);
        // call the new stream's open function
        if (stream.stream_ops.open) {
          stream.stream_ops.open(stream);
        }
        if (Module['logReadFiles'] && !(flags & 1)) {
          if (!FS.readFiles) FS.readFiles = {};
          if (!(path in FS.readFiles)) {
            FS.readFiles[path] = 1;
            Module['printErr']('read file: ' + path);
          }
        }
        return stream;
      },close:function (stream) {
        try {
          if (stream.stream_ops.close) {
            stream.stream_ops.close(stream);
          }
        } catch (e) {
          throw e;
        } finally {
          FS.closeStream(stream.fd);
        }
      },llseek:function (stream, offset, whence) {
        if (!stream.seekable || !stream.stream_ops.llseek) {
          throw new FS.ErrnoError(ERRNO_CODES.ESPIPE);
        }
        return stream.stream_ops.llseek(stream, offset, whence);
      },read:function (stream, buffer, offset, length, position) {
        if (length < 0 || position < 0) {
          throw new FS.ErrnoError(ERRNO_CODES.EINVAL);
        }
        if ((stream.flags & 2097155) === 1) {
          throw new FS.ErrnoError(ERRNO_CODES.EBADF);
        }
        if (FS.isDir(stream.node.mode)) {
          throw new FS.ErrnoError(ERRNO_CODES.EISDIR);
        }
        if (!stream.stream_ops.read) {
          throw new FS.ErrnoError(ERRNO_CODES.EINVAL);
        }
        var seeking = true;
        if (typeof position === 'undefined') {
          position = stream.position;
          seeking = false;
        } else if (!stream.seekable) {
          throw new FS.ErrnoError(ERRNO_CODES.ESPIPE);
        }
        var bytesRead = stream.stream_ops.read(stream, buffer, offset, length, position);
        if (!seeking) stream.position += bytesRead;
        return bytesRead;
      },write:function (stream, buffer, offset, length, position, canOwn) {
        if (length < 0 || position < 0) {
          throw new FS.ErrnoError(ERRNO_CODES.EINVAL);
        }
        if ((stream.flags & 2097155) === 0) {
          throw new FS.ErrnoError(ERRNO_CODES.EBADF);
        }
        if (FS.isDir(stream.node.mode)) {
          throw new FS.ErrnoError(ERRNO_CODES.EISDIR);
        }
        if (!stream.stream_ops.write) {
          throw new FS.ErrnoError(ERRNO_CODES.EINVAL);
        }
        var seeking = true;
        if (typeof position === 'undefined') {
          position = stream.position;
          seeking = false;
        } else if (!stream.seekable) {
          throw new FS.ErrnoError(ERRNO_CODES.ESPIPE);
        }
        if (stream.flags & 1024) {
          // seek to the end before writing in append mode
          FS.llseek(stream, 0, 2);
        }
        var bytesWritten = stream.stream_ops.write(stream, buffer, offset, length, position, canOwn);
        if (!seeking) stream.position += bytesWritten;
        return bytesWritten;
      },allocate:function (stream, offset, length) {
        if (offset < 0 || length <= 0) {
          throw new FS.ErrnoError(ERRNO_CODES.EINVAL);
        }
        if ((stream.flags & 2097155) === 0) {
          throw new FS.ErrnoError(ERRNO_CODES.EBADF);
        }
        if (!FS.isFile(stream.node.mode) && !FS.isDir(node.mode)) {
          throw new FS.ErrnoError(ERRNO_CODES.ENODEV);
        }
        if (!stream.stream_ops.allocate) {
          throw new FS.ErrnoError(ERRNO_CODES.EOPNOTSUPP);
        }
        stream.stream_ops.allocate(stream, offset, length);
      },mmap:function (stream, buffer, offset, length, position, prot, flags) {
        // TODO if PROT is PROT_WRITE, make sure we have write access
        if ((stream.flags & 2097155) === 1) {
          throw new FS.ErrnoError(ERRNO_CODES.EACCES);
        }
        if (!stream.stream_ops.mmap) {
          throw new FS.ErrnoError(ERRNO_CODES.ENODEV);
        }
        return stream.stream_ops.mmap(stream, buffer, offset, length, position, prot, flags);
      },ioctl:function (stream, cmd, arg) {
        if (!stream.stream_ops.ioctl) {
          throw new FS.ErrnoError(ERRNO_CODES.ENOTTY);
        }
        return stream.stream_ops.ioctl(stream, cmd, arg);
      },readFile:function (path, opts) {
        opts = opts || {};
        opts.flags = opts.flags || 'r';
        opts.encoding = opts.encoding || 'binary';
        if (opts.encoding !== 'utf8' && opts.encoding !== 'binary') {
          throw new Error('Invalid encoding type "' + opts.encoding + '"');
        }
        var ret;
        var stream = FS.open(path, opts.flags);
        var stat = FS.stat(path);
        var length = stat.size;
        var buf = new Uint8Array(length);
        FS.read(stream, buf, 0, length, 0);
        if (opts.encoding === 'utf8') {
          ret = '';
          var utf8 = new Runtime.UTF8Processor();
          for (var i = 0; i < length; i++) {
            ret += utf8.processCChar(buf[i]);
          }
        } else if (opts.encoding === 'binary') {
          ret = buf;
        }
        FS.close(stream);
        return ret;
      },writeFile:function (path, data, opts) {
        opts = opts || {};
        opts.flags = opts.flags || 'w';
        opts.encoding = opts.encoding || 'utf8';
        if (opts.encoding !== 'utf8' && opts.encoding !== 'binary') {
          throw new Error('Invalid encoding type "' + opts.encoding + '"');
        }
        var stream = FS.open(path, opts.flags, opts.mode);
        if (opts.encoding === 'utf8') {
          var utf8 = new Runtime.UTF8Processor();
          var buf = new Uint8Array(utf8.processJSString(data));
          FS.write(stream, buf, 0, buf.length, 0, opts.canOwn);
        } else if (opts.encoding === 'binary') {
          FS.write(stream, data, 0, data.length, 0, opts.canOwn);
        }
        FS.close(stream);
      },cwd:function () {
        return FS.currentPath;
      },chdir:function (path) {
        var lookup = FS.lookupPath(path, { follow: true });
        if (!FS.isDir(lookup.node.mode)) {
          throw new FS.ErrnoError(ERRNO_CODES.ENOTDIR);
        }
        var err = FS.nodePermissions(lookup.node, 'x');
        if (err) {
          throw new FS.ErrnoError(err);
        }
        FS.currentPath = lookup.path;
      },createDefaultDirectories:function () {
        FS.mkdir('/tmp');
      },createDefaultDevices:function () {
        // create /dev
        FS.mkdir('/dev');
        // setup /dev/null
        FS.registerDevice(FS.makedev(1, 3), {
          read: function() { return 0; },
          write: function() { return 0; }
        });
        FS.mkdev('/dev/null', FS.makedev(1, 3));
        // setup /dev/tty and /dev/tty1
        // stderr needs to print output using Module['printErr']
        // so we register a second tty just for it.
        TTY.register(FS.makedev(5, 0), TTY.default_tty_ops);
        TTY.register(FS.makedev(6, 0), TTY.default_tty1_ops);
        FS.mkdev('/dev/tty', FS.makedev(5, 0));
        FS.mkdev('/dev/tty1', FS.makedev(6, 0));
        // we're not going to emulate the actual shm device,
        // just create the tmp dirs that reside in it commonly
        FS.mkdir('/dev/shm');
        FS.mkdir('/dev/shm/tmp');
      },createStandardStreams:function () {
        // TODO deprecate the old functionality of a single
        // input / output callback and that utilizes FS.createDevice
        // and instead require a unique set of stream ops
  
        // by default, we symlink the standard streams to the
        // default tty devices. however, if the standard streams
        // have been overwritten we create a unique device for
        // them instead.
        if (Module['stdin']) {
          FS.createDevice('/dev', 'stdin', Module['stdin']);
        } else {
          FS.symlink('/dev/tty', '/dev/stdin');
        }
        if (Module['stdout']) {
          FS.createDevice('/dev', 'stdout', null, Module['stdout']);
        } else {
          FS.symlink('/dev/tty', '/dev/stdout');
        }
        if (Module['stderr']) {
          FS.createDevice('/dev', 'stderr', null, Module['stderr']);
        } else {
          FS.symlink('/dev/tty1', '/dev/stderr');
        }
  
        // open default streams for the stdin, stdout and stderr devices
        var stdin = FS.open('/dev/stdin', 'r');
        HEAP32[((_stdin)>>2)]=FS.getPtrForStream(stdin);
        assert(stdin.fd === 0, 'invalid handle for stdin (' + stdin.fd + ')');
  
        var stdout = FS.open('/dev/stdout', 'w');
        HEAP32[((_stdout)>>2)]=FS.getPtrForStream(stdout);
        assert(stdout.fd === 1, 'invalid handle for stdout (' + stdout.fd + ')');
  
        var stderr = FS.open('/dev/stderr', 'w');
        HEAP32[((_stderr)>>2)]=FS.getPtrForStream(stderr);
        assert(stderr.fd === 2, 'invalid handle for stderr (' + stderr.fd + ')');
      },ensureErrnoError:function () {
        if (FS.ErrnoError) return;
        FS.ErrnoError = function ErrnoError(errno) {
          this.errno = errno;
          for (var key in ERRNO_CODES) {
            if (ERRNO_CODES[key] === errno) {
              this.code = key;
              break;
            }
          }
          this.message = ERRNO_MESSAGES[errno];
        };
        FS.ErrnoError.prototype = new Error();
        FS.ErrnoError.prototype.constructor = FS.ErrnoError;
        // Some errors may happen quite a bit, to avoid overhead we reuse them (and suffer a lack of stack info)
        [ERRNO_CODES.ENOENT].forEach(function(code) {
          FS.genericErrors[code] = new FS.ErrnoError(code);
          FS.genericErrors[code].stack = '<generic error, no stack>';
        });
      },staticInit:function () {
        FS.ensureErrnoError();
  
        FS.nameTable = new Array(4096);
  
        FS.mount(MEMFS, {}, '/');
  
        FS.createDefaultDirectories();
        FS.createDefaultDevices();
      },init:function (input, output, error) {
        assert(!FS.init.initialized, 'FS.init was previously called. If you want to initialize later with custom parameters, remove any earlier calls (note that one is automatically added to the generated code)');
        FS.init.initialized = true;
  
        FS.ensureErrnoError();
  
        // Allow Module.stdin etc. to provide defaults, if none explicitly passed to us here
        Module['stdin'] = input || Module['stdin'];
        Module['stdout'] = output || Module['stdout'];
        Module['stderr'] = error || Module['stderr'];
  
        FS.createStandardStreams();
      },quit:function () {
        FS.init.initialized = false;
        for (var i = 0; i < FS.streams.length; i++) {
          var stream = FS.streams[i];
          if (!stream) {
            continue;
          }
          FS.close(stream);
        }
      },getMode:function (canRead, canWrite) {
        var mode = 0;
        if (canRead) mode |= 292 | 73;
        if (canWrite) mode |= 146;
        return mode;
      },joinPath:function (parts, forceRelative) {
        var path = PATH.join.apply(null, parts);
        if (forceRelative && path[0] == '/') path = path.substr(1);
        return path;
      },absolutePath:function (relative, base) {
        return PATH.resolve(base, relative);
      },standardizePath:function (path) {
        return PATH.normalize(path);
      },findObject:function (path, dontResolveLastLink) {
        var ret = FS.analyzePath(path, dontResolveLastLink);
        if (ret.exists) {
          return ret.object;
        } else {
          ___setErrNo(ret.error);
          return null;
        }
      },analyzePath:function (path, dontResolveLastLink) {
        // operate from within the context of the symlink's target
        try {
          var lookup = FS.lookupPath(path, { follow: !dontResolveLastLink });
          path = lookup.path;
        } catch (e) {
        }
        var ret = {
          isRoot: false, exists: false, error: 0, name: null, path: null, object: null,
          parentExists: false, parentPath: null, parentObject: null
        };
        try {
          var lookup = FS.lookupPath(path, { parent: true });
          ret.parentExists = true;
          ret.parentPath = lookup.path;
          ret.parentObject = lookup.node;
          ret.name = PATH.basename(path);
          lookup = FS.lookupPath(path, { follow: !dontResolveLastLink });
          ret.exists = true;
          ret.path = lookup.path;
          ret.object = lookup.node;
          ret.name = lookup.node.name;
          ret.isRoot = lookup.path === '/';
        } catch (e) {
          ret.error = e.errno;
        };
        return ret;
      },createFolder:function (parent, name, canRead, canWrite) {
        var path = PATH.join2(typeof parent === 'string' ? parent : FS.getPath(parent), name);
        var mode = FS.getMode(canRead, canWrite);
        return FS.mkdir(path, mode);
      },createPath:function (parent, path, canRead, canWrite) {
        parent = typeof parent === 'string' ? parent : FS.getPath(parent);
        var parts = path.split('/').reverse();
        while (parts.length) {
          var part = parts.pop();
          if (!part) continue;
          var current = PATH.join2(parent, part);
          try {
            FS.mkdir(current);
          } catch (e) {
            // ignore EEXIST
          }
          parent = current;
        }
        return current;
      },createFile:function (parent, name, properties, canRead, canWrite) {
        var path = PATH.join2(typeof parent === 'string' ? parent : FS.getPath(parent), name);
        var mode = FS.getMode(canRead, canWrite);
        return FS.create(path, mode);
      },createDataFile:function (parent, name, data, canRead, canWrite, canOwn) {
        var path = name ? PATH.join2(typeof parent === 'string' ? parent : FS.getPath(parent), name) : parent;
        var mode = FS.getMode(canRead, canWrite);
        var node = FS.create(path, mode);
        if (data) {
          if (typeof data === 'string') {
            var arr = new Array(data.length);
            for (var i = 0, len = data.length; i < len; ++i) arr[i] = data.charCodeAt(i);
            data = arr;
          }
          // make sure we can write to the file
          FS.chmod(node, mode | 146);
          var stream = FS.open(node, 'w');
          FS.write(stream, data, 0, data.length, 0, canOwn);
          FS.close(stream);
          FS.chmod(node, mode);
        }
        return node;
      },createDevice:function (parent, name, input, output) {
        var path = PATH.join2(typeof parent === 'string' ? parent : FS.getPath(parent), name);
        var mode = FS.getMode(!!input, !!output);
        if (!FS.createDevice.major) FS.createDevice.major = 64;
        var dev = FS.makedev(FS.createDevice.major++, 0);
        // Create a fake device that a set of stream ops to emulate
        // the old behavior.
        FS.registerDevice(dev, {
          open: function(stream) {
            stream.seekable = false;
          },
          close: function(stream) {
            // flush any pending line data
            if (output && output.buffer && output.buffer.length) {
              output(10);
            }
          },
          read: function(stream, buffer, offset, length, pos /* ignored */) {
            var bytesRead = 0;
            for (var i = 0; i < length; i++) {
              var result;
              try {
                result = input();
              } catch (e) {
                throw new FS.ErrnoError(ERRNO_CODES.EIO);
              }
              if (result === undefined && bytesRead === 0) {
                throw new FS.ErrnoError(ERRNO_CODES.EAGAIN);
              }
              if (result === null || result === undefined) break;
              bytesRead++;
              buffer[offset+i] = result;
            }
            if (bytesRead) {
              stream.node.timestamp = Date.now();
            }
            return bytesRead;
          },
          write: function(stream, buffer, offset, length, pos) {
            for (var i = 0; i < length; i++) {
              try {
                output(buffer[offset+i]);
              } catch (e) {
                throw new FS.ErrnoError(ERRNO_CODES.EIO);
              }
            }
            if (length) {
              stream.node.timestamp = Date.now();
            }
            return i;
          }
        });
        return FS.mkdev(path, mode, dev);
      },createLink:function (parent, name, target, canRead, canWrite) {
        var path = PATH.join2(typeof parent === 'string' ? parent : FS.getPath(parent), name);
        return FS.symlink(target, path);
      },forceLoadFile:function (obj) {
        if (obj.isDevice || obj.isFolder || obj.link || obj.contents) return true;
        var success = true;
        if (typeof XMLHttpRequest !== 'undefined') {
          throw new Error("Lazy loading should have been performed (contents set) in createLazyFile, but it was not. Lazy loading only works in web workers. Use --embed-file or --preload-file in emcc on the main thread.");
        } else if (Module['read']) {
          // Command-line.
          try {
            // WARNING: Can't read binary files in V8's d8 or tracemonkey's js, as
            //          read() will try to parse UTF8.
            obj.contents = intArrayFromString(Module['read'](obj.url), true);
          } catch (e) {
            success = false;
          }
        } else {
          throw new Error('Cannot load without read() or XMLHttpRequest.');
        }
        if (!success) ___setErrNo(ERRNO_CODES.EIO);
        return success;
      },createLazyFile:function (parent, name, url, canRead, canWrite) {
        if (typeof XMLHttpRequest !== 'undefined') {
          if (!ENVIRONMENT_IS_WORKER) throw 'Cannot do synchronous binary XHRs outside webworkers in modern browsers. Use --embed-file or --preload-file in emcc';
          // Lazy chunked Uint8Array (implements get and length from Uint8Array). Actual getting is abstracted away for eventual reuse.
          function LazyUint8Array() {
            this.lengthKnown = false;
            this.chunks = []; // Loaded chunks. Index is the chunk number
          }
          LazyUint8Array.prototype.get = function LazyUint8Array_get(idx) {
            if (idx > this.length-1 || idx < 0) {
              return undefined;
            }
            var chunkOffset = idx % this.chunkSize;
            var chunkNum = Math.floor(idx / this.chunkSize);
            return this.getter(chunkNum)[chunkOffset];
          }
          LazyUint8Array.prototype.setDataGetter = function LazyUint8Array_setDataGetter(getter) {
            this.getter = getter;
          }
          LazyUint8Array.prototype.cacheLength = function LazyUint8Array_cacheLength() {
              // Find length
              var xhr = new XMLHttpRequest();
              xhr.open('HEAD', url, false);
              xhr.send(null);
              if (!(xhr.status >= 200 && xhr.status < 300 || xhr.status === 304)) throw new Error("Couldn't load " + url + ". Status: " + xhr.status);
              var datalength = Number(xhr.getResponseHeader("Content-length"));
              var header;
              var hasByteServing = (header = xhr.getResponseHeader("Accept-Ranges")) && header === "bytes";
              var chunkSize = 1024*1024; // Chunk size in bytes
  
              if (!hasByteServing) chunkSize = datalength;
  
              // Function to get a range from the remote URL.
              var doXHR = (function(from, to) {
                if (from > to) throw new Error("invalid range (" + from + ", " + to + ") or no bytes requested!");
                if (to > datalength-1) throw new Error("only " + datalength + " bytes available! programmer error!");
  
                // TODO: Use mozResponseArrayBuffer, responseStream, etc. if available.
                var xhr = new XMLHttpRequest();
                xhr.open('GET', url, false);
                if (datalength !== chunkSize) xhr.setRequestHeader("Range", "bytes=" + from + "-" + to);
  
                // Some hints to the browser that we want binary data.
                if (typeof Uint8Array != 'undefined') xhr.responseType = 'arraybuffer';
                if (xhr.overrideMimeType) {
                  xhr.overrideMimeType('text/plain; charset=x-user-defined');
                }
  
                xhr.send(null);
                if (!(xhr.status >= 200 && xhr.status < 300 || xhr.status === 304)) throw new Error("Couldn't load " + url + ". Status: " + xhr.status);
                if (xhr.response !== undefined) {
                  return new Uint8Array(xhr.response || []);
                } else {
                  return intArrayFromString(xhr.responseText || '', true);
                }
              });
              var lazyArray = this;
              lazyArray.setDataGetter(function(chunkNum) {
                var start = chunkNum * chunkSize;
                var end = (chunkNum+1) * chunkSize - 1; // including this byte
                end = Math.min(end, datalength-1); // if datalength-1 is selected, this is the last block
                if (typeof(lazyArray.chunks[chunkNum]) === "undefined") {
                  lazyArray.chunks[chunkNum] = doXHR(start, end);
                }
                if (typeof(lazyArray.chunks[chunkNum]) === "undefined") throw new Error("doXHR failed!");
                return lazyArray.chunks[chunkNum];
              });
  
              this._length = datalength;
              this._chunkSize = chunkSize;
              this.lengthKnown = true;
          }
  
          var lazyArray = new LazyUint8Array();
          Object.defineProperty(lazyArray, "length", {
              get: function() {
                  if(!this.lengthKnown) {
                      this.cacheLength();
                  }
                  return this._length;
              }
          });
          Object.defineProperty(lazyArray, "chunkSize", {
              get: function() {
                  if(!this.lengthKnown) {
                      this.cacheLength();
                  }
                  return this._chunkSize;
              }
          });
  
          var properties = { isDevice: false, contents: lazyArray };
        } else {
          var properties = { isDevice: false, url: url };
        }
  
        var node = FS.createFile(parent, name, properties, canRead, canWrite);
        // This is a total hack, but I want to get this lazy file code out of the
        // core of MEMFS. If we want to keep this lazy file concept I feel it should
        // be its own thin LAZYFS proxying calls to MEMFS.
        if (properties.contents) {
          node.contents = properties.contents;
        } else if (properties.url) {
          node.contents = null;
          node.url = properties.url;
        }
        // override each stream op with one that tries to force load the lazy file first
        var stream_ops = {};
        var keys = Object.keys(node.stream_ops);
        keys.forEach(function(key) {
          var fn = node.stream_ops[key];
          stream_ops[key] = function forceLoadLazyFile() {
            if (!FS.forceLoadFile(node)) {
              throw new FS.ErrnoError(ERRNO_CODES.EIO);
            }
            return fn.apply(null, arguments);
          };
        });
        // use a custom read function
        stream_ops.read = function stream_ops_read(stream, buffer, offset, length, position) {
          if (!FS.forceLoadFile(node)) {
            throw new FS.ErrnoError(ERRNO_CODES.EIO);
          }
          var contents = stream.node.contents;
          if (position >= contents.length)
            return 0;
          var size = Math.min(contents.length - position, length);
          assert(size >= 0);
          if (contents.slice) { // normal array
            for (var i = 0; i < size; i++) {
              buffer[offset + i] = contents[position + i];
            }
          } else {
            for (var i = 0; i < size; i++) { // LazyUint8Array from sync binary XHR
              buffer[offset + i] = contents.get(position + i);
            }
          }
          return size;
        };
        node.stream_ops = stream_ops;
        return node;
      },createPreloadedFile:function (parent, name, url, canRead, canWrite, onload, onerror, dontCreateFile, canOwn) {
        Browser.init();
        // TODO we should allow people to just pass in a complete filename instead
        // of parent and name being that we just join them anyways
        var fullname = name ? PATH.resolve(PATH.join2(parent, name)) : parent;
        function processData(byteArray) {
          function finish(byteArray) {
            if (!dontCreateFile) {
              FS.createDataFile(parent, name, byteArray, canRead, canWrite, canOwn);
            }
            if (onload) onload();
            removeRunDependency('cp ' + fullname);
          }
          var handled = false;
          Module['preloadPlugins'].forEach(function(plugin) {
            if (handled) return;
            if (plugin['canHandle'](fullname)) {
              plugin['handle'](byteArray, fullname, finish, function() {
                if (onerror) onerror();
                removeRunDependency('cp ' + fullname);
              });
              handled = true;
            }
          });
          if (!handled) finish(byteArray);
        }
        addRunDependency('cp ' + fullname);
        if (typeof url == 'string') {
          Browser.asyncLoad(url, function(byteArray) {
            processData(byteArray);
          }, onerror);
        } else {
          processData(url);
        }
      },indexedDB:function () {
        return window.indexedDB || window.mozIndexedDB || window.webkitIndexedDB || window.msIndexedDB;
      },DB_NAME:function () {
        return 'EM_FS_' + window.location.pathname;
      },DB_VERSION:20,DB_STORE_NAME:"FILE_DATA",saveFilesToDB:function (paths, onload, onerror) {
        onload = onload || function(){};
        onerror = onerror || function(){};
        var indexedDB = FS.indexedDB();
        try {
          var openRequest = indexedDB.open(FS.DB_NAME(), FS.DB_VERSION);
        } catch (e) {
          return onerror(e);
        }
        openRequest.onupgradeneeded = function openRequest_onupgradeneeded() {
          console.log('creating db');
          var db = openRequest.result;
          db.createObjectStore(FS.DB_STORE_NAME);
        };
        openRequest.onsuccess = function openRequest_onsuccess() {
          var db = openRequest.result;
          var transaction = db.transaction([FS.DB_STORE_NAME], 'readwrite');
          var files = transaction.objectStore(FS.DB_STORE_NAME);
          var ok = 0, fail = 0, total = paths.length;
          function finish() {
            if (fail == 0) onload(); else onerror();
          }
          paths.forEach(function(path) {
            var putRequest = files.put(FS.analyzePath(path).object.contents, path);
            putRequest.onsuccess = function putRequest_onsuccess() { ok++; if (ok + fail == total) finish() };
            putRequest.onerror = function putRequest_onerror() { fail++; if (ok + fail == total) finish() };
          });
          transaction.onerror = onerror;
        };
        openRequest.onerror = onerror;
      },loadFilesFromDB:function (paths, onload, onerror) {
        onload = onload || function(){};
        onerror = onerror || function(){};
        var indexedDB = FS.indexedDB();
        try {
          var openRequest = indexedDB.open(FS.DB_NAME(), FS.DB_VERSION);
        } catch (e) {
          return onerror(e);
        }
        openRequest.onupgradeneeded = onerror; // no database to load from
        openRequest.onsuccess = function openRequest_onsuccess() {
          var db = openRequest.result;
          try {
            var transaction = db.transaction([FS.DB_STORE_NAME], 'readonly');
          } catch(e) {
            onerror(e);
            return;
          }
          var files = transaction.objectStore(FS.DB_STORE_NAME);
          var ok = 0, fail = 0, total = paths.length;
          function finish() {
            if (fail == 0) onload(); else onerror();
          }
          paths.forEach(function(path) {
            var getRequest = files.get(path);
            getRequest.onsuccess = function getRequest_onsuccess() {
              if (FS.analyzePath(path).exists) {
                FS.unlink(path);
              }
              FS.createDataFile(PATH.dirname(path), PATH.basename(path), getRequest.result, true, true, true);
              ok++;
              if (ok + fail == total) finish();
            };
            getRequest.onerror = function getRequest_onerror() { fail++; if (ok + fail == total) finish() };
          });
          transaction.onerror = onerror;
        };
        openRequest.onerror = onerror;
      }};var PATH={splitPath:function (filename) {
        var splitPathRe = /^(\/?|)([\s\S]*?)((?:\.{1,2}|[^\/]+?|)(\.[^.\/]*|))(?:[\/]*)$/;
        return splitPathRe.exec(filename).slice(1);
      },normalizeArray:function (parts, allowAboveRoot) {
        // if the path tries to go above the root, `up` ends up > 0
        var up = 0;
        for (var i = parts.length - 1; i >= 0; i--) {
          var last = parts[i];
          if (last === '.') {
            parts.splice(i, 1);
          } else if (last === '..') {
            parts.splice(i, 1);
            up++;
          } else if (up) {
            parts.splice(i, 1);
            up--;
          }
        }
        // if the path is allowed to go above the root, restore leading ..s
        if (allowAboveRoot) {
          for (; up--; up) {
            parts.unshift('..');
          }
        }
        return parts;
      },normalize:function (path) {
        var isAbsolute = path.charAt(0) === '/',
            trailingSlash = path.substr(-1) === '/';
        // Normalize the path
        path = PATH.normalizeArray(path.split('/').filter(function(p) {
          return !!p;
        }), !isAbsolute).join('/');
        if (!path && !isAbsolute) {
          path = '.';
        }
        if (path && trailingSlash) {
          path += '/';
        }
        return (isAbsolute ? '/' : '') + path;
      },dirname:function (path) {
        var result = PATH.splitPath(path),
            root = result[0],
            dir = result[1];
        if (!root && !dir) {
          // No dirname whatsoever
          return '.';
        }
        if (dir) {
          // It has a dirname, strip trailing slash
          dir = dir.substr(0, dir.length - 1);
        }
        return root + dir;
      },basename:function (path) {
        // EMSCRIPTEN return '/'' for '/', not an empty string
        if (path === '/') return '/';
        var lastSlash = path.lastIndexOf('/');
        if (lastSlash === -1) return path;
        return path.substr(lastSlash+1);
      },extname:function (path) {
        return PATH.splitPath(path)[3];
      },join:function () {
        var paths = Array.prototype.slice.call(arguments, 0);
        return PATH.normalize(paths.join('/'));
      },join2:function (l, r) {
        return PATH.normalize(l + '/' + r);
      },resolve:function () {
        var resolvedPath = '',
          resolvedAbsolute = false;
        for (var i = arguments.length - 1; i >= -1 && !resolvedAbsolute; i--) {
          var path = (i >= 0) ? arguments[i] : FS.cwd();
          // Skip empty and invalid entries
          if (typeof path !== 'string') {
            throw new TypeError('Arguments to path.resolve must be strings');
          } else if (!path) {
            continue;
          }
          resolvedPath = path + '/' + resolvedPath;
          resolvedAbsolute = path.charAt(0) === '/';
        }
        // At this point the path should be resolved to a full absolute path, but
        // handle relative paths to be safe (might happen when process.cwd() fails)
        resolvedPath = PATH.normalizeArray(resolvedPath.split('/').filter(function(p) {
          return !!p;
        }), !resolvedAbsolute).join('/');
        return ((resolvedAbsolute ? '/' : '') + resolvedPath) || '.';
      },relative:function (from, to) {
        from = PATH.resolve(from).substr(1);
        to = PATH.resolve(to).substr(1);
        function trim(arr) {
          var start = 0;
          for (; start < arr.length; start++) {
            if (arr[start] !== '') break;
          }
          var end = arr.length - 1;
          for (; end >= 0; end--) {
            if (arr[end] !== '') break;
          }
          if (start > end) return [];
          return arr.slice(start, end - start + 1);
        }
        var fromParts = trim(from.split('/'));
        var toParts = trim(to.split('/'));
        var length = Math.min(fromParts.length, toParts.length);
        var samePartsLength = length;
        for (var i = 0; i < length; i++) {
          if (fromParts[i] !== toParts[i]) {
            samePartsLength = i;
            break;
          }
        }
        var outputParts = [];
        for (var i = samePartsLength; i < fromParts.length; i++) {
          outputParts.push('..');
        }
        outputParts = outputParts.concat(toParts.slice(samePartsLength));
        return outputParts.join('/');
      }};var Browser={mainLoop:{scheduler:null,method:"",shouldPause:false,paused:false,queue:[],pause:function () {
          Browser.mainLoop.shouldPause = true;
        },resume:function () {
          if (Browser.mainLoop.paused) {
            Browser.mainLoop.paused = false;
            Browser.mainLoop.scheduler();
          }
          Browser.mainLoop.shouldPause = false;
        },updateStatus:function () {
          if (Module['setStatus']) {
            var message = Module['statusMessage'] || 'Please wait...';
            var remaining = Browser.mainLoop.remainingBlockers;
            var expected = Browser.mainLoop.expectedBlockers;
            if (remaining) {
              if (remaining < expected) {
                Module['setStatus'](message + ' (' + (expected - remaining) + '/' + expected + ')');
              } else {
                Module['setStatus'](message);
              }
            } else {
              Module['setStatus']('');
            }
          }
        }},isFullScreen:false,pointerLock:false,moduleContextCreatedCallbacks:[],workers:[],init:function () {
        if (!Module["preloadPlugins"]) Module["preloadPlugins"] = []; // needs to exist even in workers
  
        if (Browser.initted || ENVIRONMENT_IS_WORKER) return;
        Browser.initted = true;
  
        try {
          new Blob();
          Browser.hasBlobConstructor = true;
        } catch(e) {
          Browser.hasBlobConstructor = false;
          console.log("warning: no blob constructor, cannot create blobs with mimetypes");
        }
        Browser.BlobBuilder = typeof MozBlobBuilder != "undefined" ? MozBlobBuilder : (typeof WebKitBlobBuilder != "undefined" ? WebKitBlobBuilder : (!Browser.hasBlobConstructor ? console.log("warning: no BlobBuilder") : null));
        Browser.URLObject = typeof window != "undefined" ? (window.URL ? window.URL : window.webkitURL) : undefined;
        if (!Module.noImageDecoding && typeof Browser.URLObject === 'undefined') {
          console.log("warning: Browser does not support creating object URLs. Built-in browser image decoding will not be available.");
          Module.noImageDecoding = true;
        }
  
        // Support for plugins that can process preloaded files. You can add more of these to
        // your app by creating and appending to Module.preloadPlugins.
        //
        // Each plugin is asked if it can handle a file based on the file's name. If it can,
        // it is given the file's raw data. When it is done, it calls a callback with the file's
        // (possibly modified) data. For example, a plugin might decompress a file, or it
        // might create some side data structure for use later (like an Image element, etc.).
  
        var imagePlugin = {};
        imagePlugin['canHandle'] = function imagePlugin_canHandle(name) {
          return !Module.noImageDecoding && /\.(jpg|jpeg|png|bmp)$/i.test(name);
        };
        imagePlugin['handle'] = function imagePlugin_handle(byteArray, name, onload, onerror) {
          var b = null;
          if (Browser.hasBlobConstructor) {
            try {
              b = new Blob([byteArray], { type: Browser.getMimetype(name) });
              if (b.size !== byteArray.length) { // Safari bug #118630
                // Safari's Blob can only take an ArrayBuffer
                b = new Blob([(new Uint8Array(byteArray)).buffer], { type: Browser.getMimetype(name) });
              }
            } catch(e) {
              Runtime.warnOnce('Blob constructor present but fails: ' + e + '; falling back to blob builder');
            }
          }
          if (!b) {
            var bb = new Browser.BlobBuilder();
            bb.append((new Uint8Array(byteArray)).buffer); // we need to pass a buffer, and must copy the array to get the right data range
            b = bb.getBlob();
          }
          var url = Browser.URLObject.createObjectURL(b);
          var img = new Image();
          img.onload = function img_onload() {
            assert(img.complete, 'Image ' + name + ' could not be decoded');
            var canvas = document.createElement('canvas');
            canvas.width = img.width;
            canvas.height = img.height;
            var ctx = canvas.getContext('2d');
            ctx.drawImage(img, 0, 0);
            Module["preloadedImages"][name] = canvas;
            Browser.URLObject.revokeObjectURL(url);
            if (onload) onload(byteArray);
          };
          img.onerror = function img_onerror(event) {
            console.log('Image ' + url + ' could not be decoded');
            if (onerror) onerror();
          };
          img.src = url;
        };
        Module['preloadPlugins'].push(imagePlugin);
  
        var audioPlugin = {};
        audioPlugin['canHandle'] = function audioPlugin_canHandle(name) {
          return !Module.noAudioDecoding && name.substr(-4) in { '.ogg': 1, '.wav': 1, '.mp3': 1 };
        };
        audioPlugin['handle'] = function audioPlugin_handle(byteArray, name, onload, onerror) {
          var done = false;
          function finish(audio) {
            if (done) return;
            done = true;
            Module["preloadedAudios"][name] = audio;
            if (onload) onload(byteArray);
          }
          function fail() {
            if (done) return;
            done = true;
            Module["preloadedAudios"][name] = new Audio(); // empty shim
            if (onerror) onerror();
          }
          if (Browser.hasBlobConstructor) {
            try {
              var b = new Blob([byteArray], { type: Browser.getMimetype(name) });
            } catch(e) {
              return fail();
            }
            var url = Browser.URLObject.createObjectURL(b); // XXX we never revoke this!
            var audio = new Audio();
            audio.addEventListener('canplaythrough', function() { finish(audio) }, false); // use addEventListener due to chromium bug 124926
            audio.onerror = function audio_onerror(event) {
              if (done) return;
              console.log('warning: browser could not fully decode audio ' + name + ', trying slower base64 approach');
              function encode64(data) {
                var BASE = 'ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/';
                var PAD = '=';
                var ret = '';
                var leftchar = 0;
                var leftbits = 0;
                for (var i = 0; i < data.length; i++) {
                  leftchar = (leftchar << 8) | data[i];
                  leftbits += 8;
                  while (leftbits >= 6) {
                    var curr = (leftchar >> (leftbits-6)) & 0x3f;
                    leftbits -= 6;
                    ret += BASE[curr];
                  }
                }
                if (leftbits == 2) {
                  ret += BASE[(leftchar&3) << 4];
                  ret += PAD + PAD;
                } else if (leftbits == 4) {
                  ret += BASE[(leftchar&0xf) << 2];
                  ret += PAD;
                }
                return ret;
              }
              audio.src = 'data:audio/x-' + name.substr(-3) + ';base64,' + encode64(byteArray);
              finish(audio); // we don't wait for confirmation this worked - but it's worth trying
            };
            audio.src = url;
            // workaround for chrome bug 124926 - we do not always get oncanplaythrough or onerror
            Browser.safeSetTimeout(function() {
              finish(audio); // try to use it even though it is not necessarily ready to play
            }, 10000);
          } else {
            return fail();
          }
        };
        Module['preloadPlugins'].push(audioPlugin);
  
        // Canvas event setup
  
        var canvas = Module['canvas'];
        canvas.requestPointerLock = canvas['requestPointerLock'] ||
                                    canvas['mozRequestPointerLock'] ||
                                    canvas['webkitRequestPointerLock'];
        canvas.exitPointerLock = document['exitPointerLock'] ||
                                 document['mozExitPointerLock'] ||
                                 document['webkitExitPointerLock'] ||
                                 function(){}; // no-op if function does not exist
        canvas.exitPointerLock = canvas.exitPointerLock.bind(document);
  
        function pointerLockChange() {
          Browser.pointerLock = document['pointerLockElement'] === canvas ||
                                document['mozPointerLockElement'] === canvas ||
                                document['webkitPointerLockElement'] === canvas;
        }
  
        document.addEventListener('pointerlockchange', pointerLockChange, false);
        document.addEventListener('mozpointerlockchange', pointerLockChange, false);
        document.addEventListener('webkitpointerlockchange', pointerLockChange, false);
  
        if (Module['elementPointerLock']) {
          canvas.addEventListener("click", function(ev) {
            if (!Browser.pointerLock && canvas.requestPointerLock) {
              canvas.requestPointerLock();
              ev.preventDefault();
            }
          }, false);
        }
      },createContext:function (canvas, useWebGL, setInModule, webGLContextAttributes) {
        var ctx;
        var errorInfo = '?';
        function onContextCreationError(event) {
          errorInfo = event.statusMessage || errorInfo;
        }
        try {
          if (useWebGL) {
            var contextAttributes = {
              antialias: false,
              alpha: false
            };
  
            if (webGLContextAttributes) {
              for (var attribute in webGLContextAttributes) {
                contextAttributes[attribute] = webGLContextAttributes[attribute];
              }
            }
  
  
            canvas.addEventListener('webglcontextcreationerror', onContextCreationError, false);
            try {
              ['experimental-webgl', 'webgl'].some(function(webglId) {
                return ctx = canvas.getContext(webglId, contextAttributes);
              });
            } finally {
              canvas.removeEventListener('webglcontextcreationerror', onContextCreationError, false);
            }
          } else {
            ctx = canvas.getContext('2d');
          }
          if (!ctx) throw ':(';
        } catch (e) {
          Module.print('Could not create canvas: ' + [errorInfo, e]);
          return null;
        }
        if (useWebGL) {
          // Set the background of the WebGL canvas to black
          canvas.style.backgroundColor = "black";
  
          // Warn on context loss
          canvas.addEventListener('webglcontextlost', function(event) {
            alert('WebGL context lost. You will need to reload the page.');
          }, false);
        }
        if (setInModule) {
          GLctx = Module.ctx = ctx;
          Module.useWebGL = useWebGL;
          Browser.moduleContextCreatedCallbacks.forEach(function(callback) { callback() });
          Browser.init();
        }
        return ctx;
      },destroyContext:function (canvas, useWebGL, setInModule) {},fullScreenHandlersInstalled:false,lockPointer:undefined,resizeCanvas:undefined,requestFullScreen:function (lockPointer, resizeCanvas) {
        Browser.lockPointer = lockPointer;
        Browser.resizeCanvas = resizeCanvas;
        if (typeof Browser.lockPointer === 'undefined') Browser.lockPointer = true;
        if (typeof Browser.resizeCanvas === 'undefined') Browser.resizeCanvas = false;
  
        var canvas = Module['canvas'];
        function fullScreenChange() {
          Browser.isFullScreen = false;
          if ((document['webkitFullScreenElement'] || document['webkitFullscreenElement'] ||
               document['mozFullScreenElement'] || document['mozFullscreenElement'] ||
               document['fullScreenElement'] || document['fullscreenElement']) === canvas) {
            canvas.cancelFullScreen = document['cancelFullScreen'] ||
                                      document['mozCancelFullScreen'] ||
                                      document['webkitCancelFullScreen'];
            canvas.cancelFullScreen = canvas.cancelFullScreen.bind(document);
            if (Browser.lockPointer) canvas.requestPointerLock();
            Browser.isFullScreen = true;
            if (Browser.resizeCanvas) Browser.setFullScreenCanvasSize();
          } else if (Browser.resizeCanvas){
            Browser.setWindowedCanvasSize();
          }
          if (Module['onFullScreen']) Module['onFullScreen'](Browser.isFullScreen);
        }
  
        if (!Browser.fullScreenHandlersInstalled) {
          Browser.fullScreenHandlersInstalled = true;
          document.addEventListener('fullscreenchange', fullScreenChange, false);
          document.addEventListener('mozfullscreenchange', fullScreenChange, false);
          document.addEventListener('webkitfullscreenchange', fullScreenChange, false);
        }
  
        canvas.requestFullScreen = canvas['requestFullScreen'] ||
                                   canvas['mozRequestFullScreen'] ||
                                   (canvas['webkitRequestFullScreen'] ? function() { canvas['webkitRequestFullScreen'](Element['ALLOW_KEYBOARD_INPUT']) } : null);
        canvas.requestFullScreen();
      },requestAnimationFrame:function requestAnimationFrame(func) {
        if (typeof window === 'undefined') { // Provide fallback to setTimeout if window is undefined (e.g. in Node.js)
          setTimeout(func, 1000/60);
        } else {
          if (!window.requestAnimationFrame) {
            window.requestAnimationFrame = window['requestAnimationFrame'] ||
                                           window['mozRequestAnimationFrame'] ||
                                           window['webkitRequestAnimationFrame'] ||
                                           window['msRequestAnimationFrame'] ||
                                           window['oRequestAnimationFrame'] ||
                                           window['setTimeout'];
          }
          window.requestAnimationFrame(func);
        }
      },safeCallback:function (func) {
        return function() {
          if (!ABORT) return func.apply(null, arguments);
        };
      },safeRequestAnimationFrame:function (func) {
        return Browser.requestAnimationFrame(function() {
          if (!ABORT) func();
        });
      },safeSetTimeout:function (func, timeout) {
        return setTimeout(function() {
          if (!ABORT) func();
        }, timeout);
      },safeSetInterval:function (func, timeout) {
        return setInterval(function() {
          if (!ABORT) func();
        }, timeout);
      },getMimetype:function (name) {
        return {
          'jpg': 'image/jpeg',
          'jpeg': 'image/jpeg',
          'png': 'image/png',
          'bmp': 'image/bmp',
          'ogg': 'audio/ogg',
          'wav': 'audio/wav',
          'mp3': 'audio/mpeg'
        }[name.substr(name.lastIndexOf('.')+1)];
      },getUserMedia:function (func) {
        if(!window.getUserMedia) {
          window.getUserMedia = navigator['getUserMedia'] ||
                                navigator['mozGetUserMedia'];
        }
        window.getUserMedia(func);
      },getMovementX:function (event) {
        return event['movementX'] ||
               event['mozMovementX'] ||
               event['webkitMovementX'] ||
               0;
      },getMovementY:function (event) {
        return event['movementY'] ||
               event['mozMovementY'] ||
               event['webkitMovementY'] ||
               0;
      },getMouseWheelDelta:function (event) {
        return Math.max(-1, Math.min(1, event.type === 'DOMMouseScroll' ? event.detail : -event.wheelDelta));
      },mouseX:0,mouseY:0,mouseMovementX:0,mouseMovementY:0,calculateMouseEvent:function (event) { // event should be mousemove, mousedown or mouseup
        if (Browser.pointerLock) {
          // When the pointer is locked, calculate the coordinates
          // based on the movement of the mouse.
          // Workaround for Firefox bug 764498
          if (event.type != 'mousemove' &&
              ('mozMovementX' in event)) {
            Browser.mouseMovementX = Browser.mouseMovementY = 0;
          } else {
            Browser.mouseMovementX = Browser.getMovementX(event);
            Browser.mouseMovementY = Browser.getMovementY(event);
          }
          
          // check if SDL is available
          if (typeof SDL != "undefined") {
          	Browser.mouseX = SDL.mouseX + Browser.mouseMovementX;
          	Browser.mouseY = SDL.mouseY + Browser.mouseMovementY;
          } else {
          	// just add the mouse delta to the current absolut mouse position
          	// FIXME: ideally this should be clamped against the canvas size and zero
          	Browser.mouseX += Browser.mouseMovementX;
          	Browser.mouseY += Browser.mouseMovementY;
          }        
        } else {
          // Otherwise, calculate the movement based on the changes
          // in the coordinates.
          var rect = Module["canvas"].getBoundingClientRect();
          var x, y;
          
          // Neither .scrollX or .pageXOffset are defined in a spec, but
          // we prefer .scrollX because it is currently in a spec draft.
          // (see: http://www.w3.org/TR/2013/WD-cssom-view-20131217/)
          var scrollX = ((typeof window.scrollX !== 'undefined') ? window.scrollX : window.pageXOffset);
          var scrollY = ((typeof window.scrollY !== 'undefined') ? window.scrollY : window.pageYOffset);
          if (event.type == 'touchstart' ||
              event.type == 'touchend' ||
              event.type == 'touchmove') {
            var t = event.touches.item(0);
            if (t) {
              x = t.pageX - (scrollX + rect.left);
              y = t.pageY - (scrollY + rect.top);
            } else {
              return;
            }
          } else {
            x = event.pageX - (scrollX + rect.left);
            y = event.pageY - (scrollY + rect.top);
          }
  
          // the canvas might be CSS-scaled compared to its backbuffer;
          // SDL-using content will want mouse coordinates in terms
          // of backbuffer units.
          var cw = Module["canvas"].width;
          var ch = Module["canvas"].height;
          x = x * (cw / rect.width);
          y = y * (ch / rect.height);
  
          Browser.mouseMovementX = x - Browser.mouseX;
          Browser.mouseMovementY = y - Browser.mouseY;
          Browser.mouseX = x;
          Browser.mouseY = y;
        }
      },xhrLoad:function (url, onload, onerror) {
        var xhr = new XMLHttpRequest();
        xhr.open('GET', url, true);
        xhr.responseType = 'arraybuffer';
        xhr.onload = function xhr_onload() {
          if (xhr.status == 200 || (xhr.status == 0 && xhr.response)) { // file URLs can return 0
            onload(xhr.response);
          } else {
            onerror();
          }
        };
        xhr.onerror = onerror;
        xhr.send(null);
      },asyncLoad:function (url, onload, onerror, noRunDep) {
        Browser.xhrLoad(url, function(arrayBuffer) {
          assert(arrayBuffer, 'Loading data file "' + url + '" failed (no arrayBuffer).');
          onload(new Uint8Array(arrayBuffer));
          if (!noRunDep) removeRunDependency('al ' + url);
        }, function(event) {
          if (onerror) {
            onerror();
          } else {
            throw 'Loading data file "' + url + '" failed.';
          }
        });
        if (!noRunDep) addRunDependency('al ' + url);
      },resizeListeners:[],updateResizeListeners:function () {
        var canvas = Module['canvas'];
        Browser.resizeListeners.forEach(function(listener) {
          listener(canvas.width, canvas.height);
        });
      },setCanvasSize:function (width, height, noUpdates) {
        var canvas = Module['canvas'];
        canvas.width = width;
        canvas.height = height;
        if (!noUpdates) Browser.updateResizeListeners();
      },windowedWidth:0,windowedHeight:0,setFullScreenCanvasSize:function () {
        var canvas = Module['canvas'];
        this.windowedWidth = canvas.width;
        this.windowedHeight = canvas.height;
        canvas.width = screen.width;
        canvas.height = screen.height;
        // check if SDL is available   
        if (typeof SDL != "undefined") {
        	var flags = HEAPU32[((SDL.screen+Runtime.QUANTUM_SIZE*0)>>2)];
        	flags = flags | 0x00800000; // set SDL_FULLSCREEN flag
        	HEAP32[((SDL.screen+Runtime.QUANTUM_SIZE*0)>>2)]=flags
        }
        Browser.updateResizeListeners();
      },setWindowedCanvasSize:function () {
        var canvas = Module['canvas'];
        canvas.width = this.windowedWidth;
        canvas.height = this.windowedHeight;
        // check if SDL is available       
        if (typeof SDL != "undefined") {
        	var flags = HEAPU32[((SDL.screen+Runtime.QUANTUM_SIZE*0)>>2)];
        	flags = flags & ~0x00800000; // clear SDL_FULLSCREEN flag
        	HEAP32[((SDL.screen+Runtime.QUANTUM_SIZE*0)>>2)]=flags
        }
        Browser.updateResizeListeners();
      }};
___errno_state = Runtime.staticAlloc(4); HEAP32[((___errno_state)>>2)]=0;
Module["requestFullScreen"] = function Module_requestFullScreen(lockPointer, resizeCanvas) { Browser.requestFullScreen(lockPointer, resizeCanvas) };
  Module["requestAnimationFrame"] = function Module_requestAnimationFrame(func) { Browser.requestAnimationFrame(func) };
  Module["setCanvasSize"] = function Module_setCanvasSize(width, height, noUpdates) { Browser.setCanvasSize(width, height, noUpdates) };
  Module["pauseMainLoop"] = function Module_pauseMainLoop() { Browser.mainLoop.pause() };
  Module["resumeMainLoop"] = function Module_resumeMainLoop() { Browser.mainLoop.resume() };
  Module["getUserMedia"] = function Module_getUserMedia() { Browser.getUserMedia() }
FS.staticInit();__ATINIT__.unshift({ func: function() { if (!Module["noFSInit"] && !FS.init.initialized) FS.init() } });__ATMAIN__.push({ func: function() { FS.ignorePermissions = false } });__ATEXIT__.push({ func: function() { FS.quit() } });Module["FS_createFolder"] = FS.createFolder;Module["FS_createPath"] = FS.createPath;Module["FS_createDataFile"] = FS.createDataFile;Module["FS_createPreloadedFile"] = FS.createPreloadedFile;Module["FS_createLazyFile"] = FS.createLazyFile;Module["FS_createLink"] = FS.createLink;Module["FS_createDevice"] = FS.createDevice;
__ATINIT__.unshift({ func: function() { TTY.init() } });__ATEXIT__.push({ func: function() { TTY.shutdown() } });TTY.utf8 = new Runtime.UTF8Processor();
if (ENVIRONMENT_IS_NODE) { var fs = require("fs"); NODEFS.staticInit(); }
STACK_BASE = STACKTOP = Runtime.alignMemory(STATICTOP);

staticSealed = true; // seal the static portion of memory

STACK_MAX = STACK_BASE + 5242880;

DYNAMIC_BASE = DYNAMICTOP = Runtime.alignMemory(STACK_MAX);

assert(DYNAMIC_BASE < TOTAL_MEMORY, "TOTAL_MEMORY not big enough for stack");

 var ctlz_i8 = allocate([8,7,6,6,5,5,5,5,4,4,4,4,4,4,4,4,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0], "i8", ALLOC_DYNAMIC);
 var cttz_i8 = allocate([8,0,1,0,2,0,1,0,3,0,1,0,2,0,1,0,4,0,1,0,2,0,1,0,3,0,1,0,2,0,1,0,5,0,1,0,2,0,1,0,3,0,1,0,2,0,1,0,4,0,1,0,2,0,1,0,3,0,1,0,2,0,1,0,6,0,1,0,2,0,1,0,3,0,1,0,2,0,1,0,4,0,1,0,2,0,1,0,3,0,1,0,2,0,1,0,5,0,1,0,2,0,1,0,3,0,1,0,2,0,1,0,4,0,1,0,2,0,1,0,3,0,1,0,2,0,1,0,7,0,1,0,2,0,1,0,3,0,1,0,2,0,1,0,4,0,1,0,2,0,1,0,3,0,1,0,2,0,1,0,5,0,1,0,2,0,1,0,3,0,1,0,2,0,1,0,4,0,1,0,2,0,1,0,3,0,1,0,2,0,1,0,6,0,1,0,2,0,1,0,3,0,1,0,2,0,1,0,4,0,1,0,2,0,1,0,3,0,1,0,2,0,1,0,5,0,1,0,2,0,1,0,3,0,1,0,2,0,1,0,4,0,1,0,2,0,1,0,3,0,1,0,2,0,1,0], "i8", ALLOC_DYNAMIC);

var Math_min = Math.min;
function invoke_viiiii(index,a1,a2,a3,a4,a5) {
  try {
    Module["dynCall_viiiii"](index,a1,a2,a3,a4,a5);
  } catch(e) {
    if (typeof e !== 'number' && e !== 'longjmp') throw e;
    asm["setThrew"](1, 0);
  }
}

function invoke_fiiiiif(index,a1,a2,a3,a4,a5,a6) {
  try {
    return Module["dynCall_fiiiiif"](index,a1,a2,a3,a4,a5,a6);
  } catch(e) {
    if (typeof e !== 'number' && e !== 'longjmp') throw e;
    asm["setThrew"](1, 0);
  }
}

function invoke_vi(index,a1) {
  try {
    Module["dynCall_vi"](index,a1);
  } catch(e) {
    if (typeof e !== 'number' && e !== 'longjmp') throw e;
    asm["setThrew"](1, 0);
  }
}

function invoke_vii(index,a1,a2) {
  try {
    Module["dynCall_vii"](index,a1,a2);
  } catch(e) {
    if (typeof e !== 'number' && e !== 'longjmp') throw e;
    asm["setThrew"](1, 0);
  }
}

function invoke_iiii(index,a1,a2,a3) {
  try {
    return Module["dynCall_iiii"](index,a1,a2,a3);
  } catch(e) {
    if (typeof e !== 'number' && e !== 'longjmp') throw e;
    asm["setThrew"](1, 0);
  }
}

function invoke_ii(index,a1) {
  try {
    return Module["dynCall_ii"](index,a1);
  } catch(e) {
    if (typeof e !== 'number' && e !== 'longjmp') throw e;
    asm["setThrew"](1, 0);
  }
}

function invoke_fif(index,a1,a2) {
  try {
    return Module["dynCall_fif"](index,a1,a2);
  } catch(e) {
    if (typeof e !== 'number' && e !== 'longjmp') throw e;
    asm["setThrew"](1, 0);
  }
}

function invoke_viii(index,a1,a2,a3) {
  try {
    Module["dynCall_viii"](index,a1,a2,a3);
  } catch(e) {
    if (typeof e !== 'number' && e !== 'longjmp') throw e;
    asm["setThrew"](1, 0);
  }
}

function invoke_v(index) {
  try {
    Module["dynCall_v"](index);
  } catch(e) {
    if (typeof e !== 'number' && e !== 'longjmp') throw e;
    asm["setThrew"](1, 0);
  }
}

function invoke_iiiii(index,a1,a2,a3,a4) {
  try {
    return Module["dynCall_iiiii"](index,a1,a2,a3,a4);
  } catch(e) {
    if (typeof e !== 'number' && e !== 'longjmp') throw e;
    asm["setThrew"](1, 0);
  }
}

function invoke_viif(index,a1,a2,a3) {
  try {
    Module["dynCall_viif"](index,a1,a2,a3);
  } catch(e) {
    if (typeof e !== 'number' && e !== 'longjmp') throw e;
    asm["setThrew"](1, 0);
  }
}

function invoke_viiiiii(index,a1,a2,a3,a4,a5,a6) {
  try {
    Module["dynCall_viiiiii"](index,a1,a2,a3,a4,a5,a6);
  } catch(e) {
    if (typeof e !== 'number' && e !== 'longjmp') throw e;
    asm["setThrew"](1, 0);
  }
}

function invoke_iii(index,a1,a2) {
  try {
    return Module["dynCall_iii"](index,a1,a2);
  } catch(e) {
    if (typeof e !== 'number' && e !== 'longjmp') throw e;
    asm["setThrew"](1, 0);
  }
}

function invoke_iiiiii(index,a1,a2,a3,a4,a5) {
  try {
    return Module["dynCall_iiiiii"](index,a1,a2,a3,a4,a5);
  } catch(e) {
    if (typeof e !== 'number' && e !== 'longjmp') throw e;
    asm["setThrew"](1, 0);
  }
}

function invoke_fiiiif(index,a1,a2,a3,a4,a5) {
  try {
    return Module["dynCall_fiiiif"](index,a1,a2,a3,a4,a5);
  } catch(e) {
    if (typeof e !== 'number' && e !== 'longjmp') throw e;
    asm["setThrew"](1, 0);
  }
}

function invoke_viiii(index,a1,a2,a3,a4) {
  try {
    Module["dynCall_viiii"](index,a1,a2,a3,a4);
  } catch(e) {
    if (typeof e !== 'number' && e !== 'longjmp') throw e;
    asm["setThrew"](1, 0);
  }
}

function asmPrintInt(x, y) {
  Module.print('int ' + x + ',' + y);// + ' ' + new Error().stack);
}
function asmPrintFloat(x, y) {
  Module.print('float ' + x + ',' + y);// + ' ' + new Error().stack);
}
// EMSCRIPTEN_START_ASM
var asm=(function(global,env,buffer){"use asm";var a=new global.Int8Array(buffer);var b=new global.Int16Array(buffer);var c=new global.Int32Array(buffer);var d=new global.Uint8Array(buffer);var e=new global.Uint16Array(buffer);var f=new global.Uint32Array(buffer);var g=new global.Float32Array(buffer);var h=new global.Float64Array(buffer);var i=env.STACKTOP|0;var j=env.STACK_MAX|0;var k=env.tempDoublePtr|0;var l=env.ABORT|0;var m=env.cttz_i8|0;var n=env.ctlz_i8|0;var o=env.__ZTVN10__cxxabiv120__si_class_type_infoE|0;var p=env.__ZTVN10__cxxabiv117__class_type_infoE|0;var q=+env.NaN;var r=+env.Infinity;var s=0;var t=0;var u=0;var v=0;var w=0,x=0,y=0,z=0,A=0.0,B=0,C=0,D=0,E=0.0;var F=0;var G=0;var H=0;var I=0;var J=0;var K=0;var L=0;var M=0;var N=0;var O=0;var P=global.Math.floor;var Q=global.Math.abs;var R=global.Math.sqrt;var S=global.Math.pow;var T=global.Math.cos;var U=global.Math.sin;var V=global.Math.tan;var W=global.Math.acos;var X=global.Math.asin;var Y=global.Math.atan;var Z=global.Math.atan2;var _=global.Math.exp;var $=global.Math.log;var aa=global.Math.ceil;var ba=global.Math.imul;var ca=env.abort;var da=env.assert;var ea=env.asmPrintInt;var fa=env.asmPrintFloat;var ga=env.min;var ha=env.invoke_viiiii;var ia=env.invoke_fiiiiif;var ja=env.invoke_vi;var ka=env.invoke_vii;var la=env.invoke_iiii;var ma=env.invoke_ii;var na=env.invoke_fif;var oa=env.invoke_viii;var pa=env.invoke_v;var qa=env.invoke_iiiii;var ra=env.invoke_viif;var sa=env.invoke_viiiiii;var ta=env.invoke_iii;var ua=env.invoke_iiiiii;var va=env.invoke_fiiiif;var wa=env.invoke_viiii;var xa=env._llvm_lifetime_end;var ya=env._cosf;var za=env._b2WorldRayCastCallback;var Aa=env.___cxa_call_unexpected;var Ba=env._floorf;var Ca=env.___cxa_throw;var Da=env._sinf;var Ea=env._abort;var Fa=env.___cxa_end_catch;var Ga=env._b2WorldBeginContactBody;var Ha=env.___cxa_free_exception;var Ia=env._fflush;var Ja=env._sqrtf;var Ka=env._sysconf;var La=env.___setErrNo;var Ma=env._exit;var Na=env.___cxa_find_matching_catch;var Oa=env.___cxa_allocate_exception;var Pa=env.___cxa_pure_virtual;var Qa=env._b2WorldEndContactBody;var Ra=env._time;var Sa=env.___cxa_is_number_type;var Ta=env.___cxa_does_inherit;var Ua=env.__ZSt9terminatev;var Va=env._b2WorldPreSolve;var Wa=env.___cxa_begin_catch;var Xa=env._emscripten_memcpy_big;var Ya=env.__ZSt18uncaught_exceptionv;var Za=env._b2WorldQueryAABB;var _a=env._sbrk;var $a=env.__ZNSt9exceptionD2Ev;var ab=env.___errno_location;var bb=env.___gxx_personality_v0;var cb=env._b2WorldPostSolve;var db=env._llvm_lifetime_start;var eb=env.___resumeException;var fb=env.__exit;var gb=0.0;
// EMSCRIPTEN_START_FUNCS
function yl(a){a=a|0;var d=0.0,f=0,h=0,i=0,j=0,k=0,l=0,m=0,n=0,o=0,p=0,q=0.0,r=0.0,s=0.0,t=0.0,u=0.0,v=0.0,w=0,x=0,y=0,z=0.0,A=0,B=0.0,C=0,D=0,E=0,F=0.0,G=0,H=0.0,I=0.0,J=0.0,K=0.0,L=0.0,M=0.0,N=0.0,O=0,P=0,Q=0,R=0.0,S=0.0,T=0.0,U=0.0,V=0.0,W=0.0,X=0.0,Y=0.0,Z=0.0,_=0.0,$=0.0,aa=0.0,ba=0.0,ca=0.0,da=0.0,ea=0.0,fa=0.0;d=+g[a+340>>2];f=a+236|0;if((c[f>>2]|0)>0){h=a+232|0;i=a+144|0;j=a+96|0;k=0;do{l=c[h>>2]|0;m=c[l+(k*28|0)>>2]|0;n=c[(c[i>>2]|0)+(m<<2)>>2]|0;do{if((n|0)!=0){if((c[n+12>>2]&2|0)==0){break}o=c[l+(k*28|0)+4>>2]|0;p=l+(k*28|0)+16|0;q=+g[p>>2];r=+g[p+4>>2];s=+g[l+(k*28|0)+12>>2];p=(c[j>>2]|0)+(m<<3)|0;t=+g[p>>2];u=+g[p+4>>2];p=o+88|0;v=+g[p>>2];w=o+60|0;x=o+64|0;y=o+80|0;z=+g[y>>2]+(u- +g[x>>2])*(-0.0-v);A=o+84|0;B=v*(t- +g[w>>2])+ +g[A>>2];Ck(n);C=n+56|0;v=+g[C>>2];D=n+48|0;E=n+52|0;F=q*(z-(+g[D>>2]+(u- +g[n+44>>2])*(-0.0-v)))+r*(B-(v*(t- +g[n+40>>2])+ +g[E>>2]));if(!(F<0.0)){break}Ck(n);v=+g[n+32>>2];Ck(n);B=+g[n+36>>2];Ck(n);G=n+40|0;z=+g[G>>2];if(v>0.0){H=1.0/v}else{H=0.0}if(B>0.0){I=1.0/B}else{I=0.0}B=r*(t-z)-q*(u- +g[G+4>>2]);z=+g[o+132>>2];v=+g[o+44>>2];J=+g[o+48>>2];K=z*(v*v+J*J);J=+g[o+140>>2]+K-K;if(z>0.0){L=1.0/z}else{L=0.0}if(J>0.0){M=1.0/J}else{M=0.0}J=r*(t- +g[w>>2])-q*(u- +g[x>>2]);z=H+B*B*I+L+J*M*J;if(z>0.0){N=F/z}else{N=0.0}z=d*(s<1.0?s:1.0)*N;s=H*z;g[D>>2]=+g[D>>2]+q*s;g[E>>2]=+g[E>>2]+r*s;g[C>>2]=I*B*z+ +g[C>>2];B=-0.0-z;z=q*B;q=r*B;if((c[o>>2]|0)!=2){break}C=o+4|0;E=b[C>>1]|0;if((E&2)==0){D=E|2;b[C>>1]=D;g[o+160>>2]=0.0;O=D}else{O=E}if((O&2)==0){break}B=+g[o+136>>2];g[y>>2]=z*B+ +g[y>>2];g[A>>2]=q*B+ +g[A>>2];g[p>>2]=+g[p>>2]+ +g[o+144>>2]*(q*(t- +g[w>>2])-z*(u- +g[x>>2]))}}while(0);k=k+1|0;}while((k|0)<(c[f>>2]|0))}f=a+220|0;if((c[f>>2]|0)<=0){return}k=a+216|0;O=a+144|0;j=a+96|0;i=a+104|0;h=a+88|0;n=a+32|0;m=a+320|0;a=0;do{l=c[k>>2]|0;x=e[l+(a*20|0)>>1]|0;w=e[l+(a*20|0)+2>>1]|0;o=l+(a*20|0)+8|0;I=+g[o>>2];H=+g[o+4>>2];N=+g[l+(a*20|0)+4>>2];l=c[O>>2]|0;o=c[l+(x<<2)>>2]|0;p=c[l+(w<<2)>>2]|0;if((o|0)==0){P=0}else{P=(c[o+12>>2]&2|0)!=0}if((p|0)==0){Q=0}else{Q=(c[p+12>>2]&2|0)!=0}do{if((o|0)!=(p|0)&(P|Q)){l=c[j>>2]|0;M=(+g[l+(x<<3)>>2]+ +g[l+(w<<3)>>2])*.5;L=(+g[l+(x<<3)+4>>2]+ +g[l+(w<<3)+4>>2])*.5;if(Q){Ck(p);u=+g[p+56>>2];R=+g[p+48>>2]+(L- +g[p+44>>2])*(-0.0-u);S=u*(M- +g[p+40>>2])+ +g[p+52>>2]}else{l=(c[i>>2]|0)+(w<<3)|0;u=+g[l>>2];R=u;S=+g[l+4>>2]}if(P){Ck(o);u=+g[o+56>>2];T=+g[o+48>>2]+(L- +g[o+44>>2])*(-0.0-u);U=u*(M- +g[o+40>>2])+ +g[o+52>>2]}else{l=(c[i>>2]|0)+(x<<3)|0;u=+g[l>>2];T=u;U=+g[l+4>>2]}u=H*(S-U)+I*(R-T);if(!(u<0.0)){break}if(P){Ck(o);z=+g[o+32>>2];Ck(o);t=+g[o+36>>2];Ck(o);l=o+40|0;q=+g[l>>2];if(z>0.0){V=1.0/z}else{V=0.0}if(t>0.0){W=1.0/t}else{W=0.0}X=H*(M-q)-I*(L- +g[l+4>>2]);Y=W;Z=V}else{do{if((c[(c[h>>2]|0)+(x<<2)>>2]&4|0)==0){q=+g[n>>2]*.75;t=q*+g[m>>2]*q;if(!(t>0.0)){_=0.0;break}_=1.0/t}else{_=0.0}}while(0);X=H*(M-M)-I*(L-L);Y=0.0;Z=_}if(Q){Ck(p);t=+g[p+32>>2];Ck(p);q=+g[p+36>>2];Ck(p);l=p+40|0;z=+g[l>>2];if(t>0.0){$=1.0/t}else{$=0.0}if(q>0.0){aa=1.0/q}else{aa=0.0}ba=H*(M-z)-I*(L- +g[l+4>>2]);ca=aa;da=$}else{do{if((c[(c[h>>2]|0)+(w<<2)>>2]&4|0)==0){z=+g[n>>2]*.75;q=z*+g[m>>2]*z;if(!(q>0.0)){ea=0.0;break}ea=1.0/q}else{ea=0.0}}while(0);ba=H*(M-M)-I*(L-L);ca=0.0;da=ea}q=Z+X*Y*X+da+ba*ca*ba;if(q>0.0){fa=u/q}else{fa=0.0}q=d*N*fa;if(P){z=Z*q;l=o+48|0;g[l>>2]=I*z+ +g[l>>2];l=o+52|0;g[l>>2]=H*z+ +g[l>>2];l=o+56|0;g[l>>2]=Y*X*q+ +g[l>>2]}else{l=c[i>>2]|0;z=Z*q;A=l+(x<<3)|0;g[A>>2]=I*z+ +g[A>>2];A=l+(x<<3)+4|0;g[A>>2]=H*z+ +g[A>>2]}z=-0.0-q;if(Q){q=da*z;A=p+48|0;g[A>>2]=I*q+ +g[A>>2];A=p+52|0;g[A>>2]=H*q+ +g[A>>2];A=p+56|0;g[A>>2]=ca*ba*z+ +g[A>>2];break}else{A=c[i>>2]|0;q=da*z;l=A+(w<<3)|0;g[l>>2]=I*q+ +g[l>>2];l=A+(w<<3)+4|0;g[l>>2]=H*q+ +g[l>>2];break}}}while(0);a=a+1|0;}while((a|0)<(c[f>>2]|0));return}function zl(a,b){a=a|0;b=b|0;var d=0,e=0,f=0,h=0,i=0.0,j=0.0,k=0.0,l=0.0,m=0.0,n=0.0,o=0.0,p=0.0,q=0.0,r=0.0,s=0,t=0;d=c[a+312>>2]|0;if((d|0)==0){return}e=b|0;f=b+4|0;b=a+104|0;h=a+96|0;a=d;do{do{if((c[a+12>>2]&2|0)!=0){Ck(a);i=+g[e>>2];j=i*+g[a+56>>2];k=+U(j);l=+T(j);j=+g[a+40>>2];m=+g[a+44>>2];n=i*+g[a+48>>2]+j-(l*j-k*m);o=i*+g[a+52>>2]+m-(k*j+l*m);d=a+60|0;m=+g[a+72>>2];j=+g[a+68>>2];i=+(k*m+l*j);p=+(l*m-k*j);j=+g[d>>2];m=+g[a+64>>2];q=+(n+(l*j-k*m));r=+(o+(k*j+l*m));s=d;g[s>>2]=q;g[s+4>>2]=r;s=a+68|0;g[s>>2]=i;g[s+4>>2]=p;p=+g[f>>2];i=n*p;n=o*p;o=k*p;k=(l+-1.0)*p;s=c[a+4>>2]|0;d=a+8|0;if((s|0)<(c[d>>2]|0)){t=s}else{break}do{s=c[h>>2]|0;p=+g[s+(t<<3)>>2];l=+g[s+(t<<3)+4>>2];s=(c[b>>2]|0)+(t<<3)|0;r=+(i+(k*p-o*l));q=+(n+(o*p+k*l));g[s>>2]=r;g[s+4>>2]=q;t=t+1|0;}while((t|0)<(c[d>>2]|0))}}while(0);a=c[a+24>>2]|0;}while((a|0)!=0);return}function Al(b,d,e,f){b=b|0;d=d|0;e=e|0;f=f|0;var h=0.0,i=0.0,j=0.0;h=+(e-d|0);i=+g[f>>2]/h;j=+g[f+4>>2]/h;if(!(i!=0.0|j!=0.0)){return}f=b+21|0;if((a[f]|0)==0){Nn(c[b+112>>2]|0,0,c[b+44>>2]<<3|0)|0;a[f]=1}if((d|0)>=(e|0)){return}f=c[b+112>>2]|0;b=d;do{d=f+(b<<3)|0;g[d>>2]=i+ +g[d>>2];d=f+(b<<3)+4|0;g[d>>2]=j+ +g[d>>2];b=b+1|0;}while((b|0)<(e|0));return}function Bl(a,b,d,e){a=a|0;b=b|0;d=d|0;e=e|0;var f=0.0,h=0.0,i=0.0;f=+g[a+32>>2]*.75;h=+(d-b|0)*f*+g[a+320>>2]*f;f=+g[e>>2]/h;i=+g[e+4>>2]/h;if((b|0)>=(d|0)){return}e=c[a+104>>2]|0;a=b;do{b=e+(a<<3)|0;g[b>>2]=f+ +g[b>>2];b=e+(a<<3)+4|0;g[b>>2]=i+ +g[b>>2];a=a+1|0;}while((a|0)<(d|0));return}function Cl(a,b,d){a=a|0;b=b|0;d=d|0;var e=0,f=0,h=0.0,i=0,j=0.0,k=0,l=0,m=0,n=0,o=0,p=0,q=0,r=0,s=0.0,t=0;e=c[a+204>>2]|0;if((e|0)==0){return}f=c[a+200>>2]|0;h=+g[a+36>>2];i=d|0;j=+g[i>>2];k=d+4|0;l=(~~(h*+g[k>>2]+2048.0)<<20)+~~(h*j*256.0+524288.0)|0;m=f;n=e<<3>>3;a:while(1){o=n;while(1){if((o|0)==0){break a}p=(o|0)/2|0;if((c[m+(p<<3)+4>>2]|0)>>>0<l>>>0){break}else{o=p}}m=m+(p+1<<3)|0;n=o-1-p|0}p=d+8|0;n=d+12|0;d=(~~(h*+g[n>>2]+2048.0)<<20)+~~(h*+g[p>>2]*256.0+524288.0)|0;l=m;q=f+(e<<3)-m>>3;b:while(1){e=q;while(1){if((e|0)==0){break b}r=(e|0)/2|0;if((c[l+(r<<3)+4>>2]|0)>>>0>d>>>0){e=r}else{break}}l=l+(r+1<<3)|0;q=e-1-r|0}if(!(m>>>0<l>>>0)){return}r=a+96|0;q=b;d=m;h=j;c:while(1){m=c[d>>2]|0;o=c[r>>2]|0;j=+g[o+(m<<3)>>2];do{if(h<j){if(!(j<+g[p>>2])){break}s=+g[o+(m<<3)+4>>2];if(!(+g[k>>2]<s)){break}if(!(s<+g[n>>2])){break}if(!(lb[c[(c[q>>2]|0)+12>>2]&31](b,a,m)|0)){t=21;break c}}}while(0);m=d+8|0;if(!(m>>>0<l>>>0)){t=21;break}d=m;h=+g[i>>2]}if((t|0)==21){return}}function Dl(a,b,d,e){a=a|0;b=b|0;d=d|0;e=e|0;var f=0,h=0,j=0,k=0,l=0,m=0,n=0,o=0.0,p=0.0,q=0,r=0.0,s=0.0,t=0.0,u=0.0,v=0,w=0,x=0,y=0,z=0,A=0,B=0,C=0,D=0,E=0,F=0,G=0,H=0,I=0,J=0.0,K=0.0,L=0.0,M=0.0,N=0.0,O=0.0,P=0.0,Q=0.0,S=0.0;f=i;i=i+56|0;h=f|0;j=f+16|0;k=f+40|0;l=f+48|0;if((c[a+204>>2]|0)==0){i=f;return}m=h|0;n=d|0;o=+g[n>>2];p=+g[e>>2];q=d+4|0;r=+g[q>>2];s=+g[e+4>>2];t=+(o<p?o:p);u=+(r<s?r:s);g[m>>2]=t;g[m+4>>2]=u;m=h+8|0;u=+(o>p?o:p);t=+(r>s?r:s);g[m>>2]=u;g[m+4>>2]=t;t=p-o;o=s-r;r=t*t+o*o;al(j,a,h);h=j+16|0;m=c[j+20>>2]|0;e=j|0;d=j+4|0;j=a+96|0;v=a+40|0;w=k|0;x=k+4|0;y=b;z=l|0;A=l+4|0;s=1.0;B=c[h>>2]|0;a:while(1){if(!(B>>>0<m>>>0)){C=16;break}D=c[e>>2]|0;E=c[d>>2]|0;F=B;b:while(1){G=c[F+4>>2]&1048575;H=F+8|0;c[h>>2]=H;if(G>>>0<D>>>0|G>>>0>E>>>0){if(H>>>0<m>>>0){F=H;continue}else{C=16;break a}}I=c[F>>2]|0;if(!((I|0)>-1)){C=16;break a}G=c[j>>2]|0;J=+g[n>>2];K=J- +g[G+(I<<3)>>2];L=+g[q>>2];M=L- +g[G+(I<<3)+4>>2];p=t*K+o*M;u=p*p-r*(K*K+M*M- +g[v>>2]);do{if(!(u<0.0)){N=+R(u);O=(-0.0-p-N)/r;if(O>s){break}if(!(O<0.0)){P=O;break b}O=(N-p)/r;if(!(O<0.0|O>s)){P=O;break b}}}while(0);if(H>>>0<m>>>0){F=H}else{C=16;break a}}p=t*P;u=o*P;O=K+p;N=M+u;g[w>>2]=O;g[x>>2]=N;Q=+R(O*O+N*N);if(!(Q<1.1920928955078125e-7)){S=1.0/Q;g[w>>2]=O*S;g[x>>2]=N*S}F=c[(c[y>>2]|0)+12>>2]|0;g[z>>2]=p+J;g[A>>2]=u+L;u=+ib[F&3](b,a,I,l,k,P);p=s<u?s:u;if(p>0.0){s=p;B=H}else{C=16;break}}if((C|0)==16){i=f;return}}function El(b,c,e){b=b|0;c=c|0;e=e|0;var f=0,g=0,h=0,i=0,j=0,k=0,l=0,m=0,n=0,o=0,p=0,q=0,r=0,s=0,t=0,u=0,v=0,w=0;f=((e-4+(-c|0)|0)>>>2)+1|0;g=b;h=c;i=c;while(1){c=g;j=d[c]|d[c+1|0]<<8|d[c+2|0]<<16|d[c+3|0]<<24|0;c=h|0;k=h+1|0;l=a[k]|0;m=h+2|0;n=a[m]|0;o=h+3|0;p=a[o]|0;a[g|0]=a[c]|0;a[g+1|0]=l;a[g+2|0]=n;a[g+3|0]=p;a[c]=j;a[k]=j>>>8;a[m]=j>>>16;a[o]=j>>>24;j=g+4|0;o=h+4|0;q=(j|0)==(i|0);if((o|0)==(e|0)){break}g=j;h=o;i=q?o:i}h=b+(f<<2)|0;if(q){return h|0}else{r=h;s=i;t=i}a:while(1){i=r;q=s;while(1){f=i;b=d[f]|d[f+1|0]<<8|d[f+2|0]<<16|d[f+3|0]<<24|0;f=q|0;g=q+1|0;o=a[g]|0;j=q+2|0;m=a[j]|0;k=q+3|0;c=a[k]|0;a[i|0]=a[f]|0;a[i+1|0]=o;a[i+2|0]=m;a[i+3|0]=c;a[f]=b;a[g]=b>>>8;a[j]=b>>>16;a[k]=b>>>24;u=i+4|0;v=q+4|0;w=(u|0)==(t|0);if((v|0)!=(e|0)){break}if(w){break a}else{i=u;q=t}}r=u;s=v;t=w?v:t}return h|0}function Fl(a,b,d){a=a|0;b=b|0;d=d|0;var e=0,f=0,g=0,h=0,i=0,j=0,k=0,l=0,m=0,n=0,o=0,p=0,q=0,r=0.0,s=0,t=0,u=0,v=0,w=0,x=0,y=0,z=0,A=0,B=0,C=0,D=0,E=0,F=0,G=0,H=0,I=0,J=0,K=0,L=0,M=0,N=0,O=0,P=0,Q=0,R=0,S=0,T=0,U=0,V=0,W=0;e=d|0;f=a;a=b;a:while(1){b=a;g=a-4|0;h=a-8|0;i=f;b:while(1){j=i;k=b-j|0;l=k>>2;switch(l|0){case 4:{m=9;break a;break};case 2:{m=4;break a;break};case 0:case 1:{m=80;break a;break};case 5:{m=10;break a;break};case 3:{m=8;break a;break};default:{}}if((k|0)<124){m=12;break a}n=(l|0)/2|0;o=i+(n<<2)|0;if((k|0)>3996){k=(l|0)/4|0;p=Il(i,i+(k<<2)|0,o,i+(k+n<<2)|0,g,d)|0}else{p=Gl(i,o,g,c[e>>2]|0)|0}n=c[i>>2]|0;k=c[e>>2]|0;l=c[k+(n<<2)>>2]|0;q=c[k+(c[o>>2]<<2)>>2]|0;r=+(l|0);s=r<=0.0;t=+(q|0)<=0.0;if(s^t){if(s){u=g;v=p}else{m=30}}else{if((l|0)>(q|0)){u=g;v=p}else{m=30}}c:do{if((m|0)==30){m=0;d:do{if((i|0)!=(h|0)){w=h;while(1){x=c[w>>2]|0;y=c[k+(x<<2)>>2]|0;z=+(y|0)<=0.0;if(z^t){if(z){break}}else{if((y|0)>(q|0)){break}}y=w-4|0;if((i|0)==(y|0)){break d}else{w=y}}c[i>>2]=x;c[w>>2]=n;u=w;v=p+1|0;break c}}while(0);y=i+4|0;z=c[g>>2]|0;A=c[k+(z<<2)>>2]|0;if(s^+(A|0)<=0.0){if(s){B=y}else{m=34}}else{if((l|0)>(A|0)){B=y}else{m=34}}if((m|0)==34){m=0;if((y|0)==(g|0)){m=80;break a}e:do{if(r>0.0){A=y;while(1){C=c[A>>2]|0;D=c[k+(C<<2)>>2]|0;if(+(D|0)>0.0&(l|0)>(D|0)){E=A;F=C;break e}C=A+4|0;if((C|0)==(g|0)){m=80;break a}else{A=C}}}else{A=y;while(1){w=c[A>>2]|0;C=c[k+(w<<2)>>2]|0;if(+(C|0)>0.0|(l|0)>(C|0)){E=A;F=w;break e}w=A+4|0;if((w|0)==(g|0)){m=80;break a}else{A=w}}}}while(0);c[E>>2]=z;c[g>>2]=F;B=E+4|0}if((B|0)==(g|0)){m=80;break a}else{G=g;H=B}while(1){y=c[k+(c[i>>2]<<2)>>2]|0;A=+(y|0)>0.0;f:do{if(A){w=H;while(1){C=c[w>>2]|0;D=c[k+(C<<2)>>2]|0;if(+(D|0)>0.0&(y|0)>(D|0)){I=w;J=C;break f}w=w+4|0}}else{w=H;while(1){C=c[w>>2]|0;D=c[k+(C<<2)>>2]|0;if(+(D|0)>0.0|(y|0)>(D|0)){I=w;J=C;break f}w=w+4|0}}}while(0);if(A){w=G;while(1){C=w-4|0;D=c[C>>2]|0;K=c[k+(D<<2)>>2]|0;if(+(K|0)>0.0&(y|0)>(K|0)){w=C}else{L=C;M=D;break}}}else{w=G;while(1){A=w-4|0;D=c[A>>2]|0;C=c[k+(D<<2)>>2]|0;if(+(C|0)>0.0|(y|0)>(C|0)){w=A}else{L=A;M=D;break}}}if(!(I>>>0<L>>>0)){i=I;continue b}c[I>>2]=M;c[L>>2]=J;G=L;H=I+4|0}}}while(0);l=i+4|0;g:do{if(l>>>0<u>>>0){s=u;n=l;q=v;t=o;while(1){z=c[k+(c[t>>2]<<2)>>2]|0;w=+(z|0)<=0.0;y=n;while(1){N=c[y>>2]|0;D=c[k+(N<<2)>>2]|0;A=+(D|0)<=0.0;if(A^w){if(!A){O=s;break}}else{if((D|0)<=(z|0)){O=s;break}}y=y+4|0}while(1){P=O-4|0;Q=c[P>>2]|0;D=c[k+(Q<<2)>>2]|0;A=+(D|0)<=0.0;if(A^w){if(A){break}else{O=P;continue}}else{if((D|0)>(z|0)){break}else{O=P;continue}}}if(y>>>0>P>>>0){R=y;S=q;T=t;break g}c[y>>2]=Q;c[P>>2]=N;s=P;n=y+4|0;q=q+1|0;t=(t|0)==(y|0)?P:t}}else{R=l;S=v;T=o}}while(0);do{if((R|0)==(T|0)){U=S}else{o=c[T>>2]|0;l=c[R>>2]|0;t=c[k+(o<<2)>>2]|0;q=c[k+(l<<2)>>2]|0;n=+(t|0)<=0.0;if(n^+(q|0)<=0.0){if(!n){U=S;break}}else{if((t|0)<=(q|0)){U=S;break}}c[R>>2]=o;c[T>>2]=l;U=S+1|0}}while(0);if((U|0)==0){V=Jl(i,R,d)|0;k=R+4|0;if(Jl(k,a,d)|0){m=75;break}if(V){i=k;continue}}k=R;if((k-j|0)>=(b-k|0)){m=79;break}Fl(i,R,d);i=R+4|0}if((m|0)==75){m=0;if(V){m=80;break}else{f=i;a=R;continue}}else if((m|0)==79){m=0;Fl(R+4|0,a,d);f=i;a=R;continue}}if((m|0)==4){R=c[g>>2]|0;f=c[i>>2]|0;V=c[e>>2]|0;U=c[V+(R<<2)>>2]|0;S=c[V+(f<<2)>>2]|0;V=+(U|0)<=0.0;do{if(V^+(S|0)<=0.0){if(V){break}return}else{if((U|0)>(S|0)){break}return}}while(0);c[i>>2]=R;c[g>>2]=f;return}else if((m|0)==8){Gl(i,i+4|0,g,c[e>>2]|0)|0;return}else if((m|0)==9){Hl(i,i+4|0,i+8|0,g,d)|0;return}else if((m|0)==10){Il(i,i+4|0,i+8|0,i+12|0,g,d)|0;return}else if((m|0)==12){d=i+8|0;Gl(i,i+4|0,d,c[e>>2]|0)|0;g=i+12|0;if((g|0)==(a|0)){return}f=c[e>>2]|0;e=d;d=g;while(1){g=c[d>>2]|0;R=c[e>>2]|0;S=f+(g<<2)|0;U=c[S>>2]|0;V=c[f+(R<<2)>>2]|0;T=+(U|0)<=0.0;if(T^+(V|0)<=0.0){if(T){m=17}}else{if((U|0)>(V|0)){m=17}}if((m|0)==17){m=0;c[d>>2]=R;h:do{if((e|0)==(i|0)){W=i}else{R=e;while(1){V=R-4|0;U=c[V>>2]|0;T=c[S>>2]|0;v=c[f+(U<<2)>>2]|0;P=+(T|0)<=0.0;if(P^+(v|0)<=0.0){if(!P){W=R;break h}}else{if((T|0)<=(v|0)){W=R;break h}}c[R>>2]=U;if((V|0)==(i|0)){W=i;break}else{R=V}}}}while(0);c[W>>2]=g}S=d+4|0;if((S|0)==(a|0)){break}else{e=d;d=S}}return}else if((m|0)==80){return}}function Gl(a,b,d,e){a=a|0;b=b|0;d=d|0;e=e|0;var f=0,g=0,h=0,i=0,j=0,k=0,l=0,m=0,n=0,o=0,p=0;f=c[b>>2]|0;g=c[a>>2]|0;h=c[e+(f<<2)>>2]|0;i=e+(g<<2)|0;j=c[i>>2]|0;k=+(h|0)<=0.0;if(k^+(j|0)<=0.0){l=k}else{l=(h|0)>(j|0)}j=c[d>>2]|0;m=c[e+(j<<2)>>2]|0;n=+(m|0)<=0.0;if(n^k){o=n}else{o=(m|0)>(h|0)}if(!l){if(!o){p=0;return p|0}c[b>>2]=j;c[d>>2]=f;l=c[b>>2]|0;h=c[a>>2]|0;m=c[e+(l<<2)>>2]|0;n=c[e+(h<<2)>>2]|0;k=+(m|0)<=0.0;do{if(k^+(n|0)<=0.0){if(k){break}else{p=1}return p|0}else{if((m|0)>(n|0)){break}else{p=1}return p|0}}while(0);c[a>>2]=l;c[b>>2]=h;p=2;return p|0}if(o){c[a>>2]=j;c[d>>2]=g;p=1;return p|0}c[a>>2]=f;c[b>>2]=g;f=c[d>>2]|0;a=c[e+(f<<2)>>2]|0;e=c[i>>2]|0;i=+(a|0)<=0.0;do{if(i^+(e|0)<=0.0){if(i){break}else{p=1}return p|0}else{if((a|0)>(e|0)){break}else{p=1}return p|0}}while(0);c[b>>2]=f;c[d>>2]=g;p=2;return p|0}function Hl(a,b,d,e,f){a=a|0;b=b|0;d=d|0;e=e|0;f=f|0;var g=0,h=0,i=0,j=0,k=0,l=0,m=0;g=f|0;f=Gl(a,b,d,c[g>>2]|0)|0;h=c[e>>2]|0;i=c[d>>2]|0;j=c[g>>2]|0;g=c[j+(h<<2)>>2]|0;k=c[j+(i<<2)>>2]|0;l=+(g|0)<=0.0;do{if(l^+(k|0)<=0.0){if(l){break}else{m=f}return m|0}else{if((g|0)>(k|0)){break}else{m=f}return m|0}}while(0);c[d>>2]=h;c[e>>2]=i;i=f+1|0;e=c[d>>2]|0;h=c[b>>2]|0;k=c[j+(e<<2)>>2]|0;g=c[j+(h<<2)>>2]|0;l=+(k|0)<=0.0;do{if(l^+(g|0)<=0.0){if(l){break}else{m=i}return m|0}else{if((k|0)>(g|0)){break}else{m=i}return m|0}}while(0);c[b>>2]=e;c[d>>2]=h;h=f+2|0;d=c[b>>2]|0;e=c[a>>2]|0;i=c[j+(d<<2)>>2]|0;g=c[j+(e<<2)>>2]|0;j=+(i|0)<=0.0;do{if(j^+(g|0)<=0.0){if(j){break}else{m=h}return m|0}else{if((i|0)>(g|0)){break}else{m=h}return m|0}}while(0);c[a>>2]=d;c[b>>2]=e;m=f+3|0;return m|0}function Il(a,b,d,e,f,g){a=a|0;b=b|0;d=d|0;e=e|0;f=f|0;g=g|0;var h=0,i=0,j=0,k=0,l=0,m=0,n=0;h=Hl(a,b,d,e,g)|0;i=c[f>>2]|0;j=c[e>>2]|0;k=c[g>>2]|0;g=c[k+(i<<2)>>2]|0;l=c[k+(j<<2)>>2]|0;m=+(g|0)<=0.0;do{if(m^+(l|0)<=0.0){if(m){break}else{n=h}return n|0}else{if((g|0)>(l|0)){break}else{n=h}return n|0}}while(0);c[e>>2]=i;c[f>>2]=j;j=h+1|0;f=c[e>>2]|0;i=c[d>>2]|0;l=c[k+(f<<2)>>2]|0;g=c[k+(i<<2)>>2]|0;m=+(l|0)<=0.0;do{if(m^+(g|0)<=0.0){if(m){break}else{n=j}return n|0}else{if((l|0)>(g|0)){break}else{n=j}return n|0}}while(0);c[d>>2]=f;c[e>>2]=i;i=h+2|0;e=c[d>>2]|0;f=c[b>>2]|0;j=c[k+(e<<2)>>2]|0;g=c[k+(f<<2)>>2]|0;l=+(j|0)<=0.0;do{if(l^+(g|0)<=0.0){if(l){break}else{n=i}return n|0}else{if((j|0)>(g|0)){break}else{n=i}return n|0}}while(0);c[b>>2]=e;c[d>>2]=f;f=h+3|0;d=c[b>>2]|0;e=c[a>>2]|0;i=c[k+(d<<2)>>2]|0;g=c[k+(e<<2)>>2]|0;k=+(i|0)<=0.0;do{if(k^+(g|0)<=0.0){if(k){break}else{n=f}return n|0}else{if((i|0)>(g|0)){break}else{n=f}return n|0}}while(0);c[a>>2]=d;c[b>>2]=e;n=h+4|0;return n|0}function Jl(a,b,d){a=a|0;b=b|0;d=d|0;var e=0,f=0,g=0,h=0,i=0,j=0,k=0,l=0,m=0,n=0,o=0,p=0,q=0,r=0,s=0;switch(b-a>>2|0){case 2:{e=b-4|0;f=c[e>>2]|0;g=c[a>>2]|0;h=c[d>>2]|0;i=c[h+(f<<2)>>2]|0;j=c[h+(g<<2)>>2]|0;h=+(i|0)<=0.0;do{if(h^+(j|0)<=0.0){if(h){break}else{k=1}return k|0}else{if((i|0)>(j|0)){break}else{k=1}return k|0}}while(0);c[a>>2]=f;c[e>>2]=g;k=1;return k|0};case 4:{Hl(a,a+4|0,a+8|0,b-4|0,d)|0;k=1;return k|0};case 0:case 1:{k=1;return k|0};case 3:{Gl(a,a+4|0,b-4|0,c[d>>2]|0)|0;k=1;return k|0};case 5:{Il(a,a+4|0,a+8|0,a+12|0,b-4|0,d)|0;k=1;return k|0};default:{g=a+8|0;e=d|0;Gl(a,a+4|0,g,c[e>>2]|0)|0;d=a+12|0;if((d|0)==(b|0)){k=1;return k|0}f=c[e>>2]|0;e=g;g=0;j=d;while(1){d=c[j>>2]|0;i=c[e>>2]|0;h=f+(d<<2)|0;l=c[h>>2]|0;m=c[f+(i<<2)>>2]|0;n=+(l|0)<=0.0;if(n^+(m|0)<=0.0){if(n){o=14}else{p=g}}else{if((l|0)>(m|0)){o=14}else{p=g}}if((o|0)==14){o=0;c[j>>2]=i;a:do{if((e|0)==(a|0)){q=a}else{i=e;while(1){m=i-4|0;l=c[m>>2]|0;n=c[h>>2]|0;r=c[f+(l<<2)>>2]|0;s=+(n|0)<=0.0;if(s^+(r|0)<=0.0){if(!s){q=i;break a}}else{if((n|0)<=(r|0)){q=i;break a}}c[i>>2]=l;if((m|0)==(a|0)){q=a;break}else{i=m}}}}while(0);c[q>>2]=d;h=g+1|0;if((h|0)==8){break}else{p=h}}h=j+4|0;if((h|0)==(b|0)){k=1;o=22;break}else{e=j;g=p;j=h}}if((o|0)==22){return k|0}k=(j+4|0)==(b|0);return k|0}}return 0}function Kl(a,b){a=a|0;b=b|0;var d=0,e=0,f=0,h=0,j=0,k=0,l=0,m=0,n=0,o=0.0,p=0.0,q=0.0,r=0;d=i;i=i+24|0;e=d|0;f=d+8|0;h=d+16|0;j=b|0;k=a+4|0;l=a+8|0;do{if((c[j>>2]|0)==(c[k>>2]|0)){m=c[l>>2]|0;c[a+8>>2]=m+1;if((m|0)<=3){break}m=c[a+12>>2]|0;c[m>>2]=(c[m>>2]|0)+1;n=1;i=d;return n|0}else{c[l>>2]=0;c[k>>2]=c[j>>2];c[a+8>>2]=1}}while(0);k=b+16|0;o=+g[k>>2];l=c[a>>2]|0;p=+g[l+32>>2]*(1.0- +g[b+12>>2]);m=c[j>>2]|0;j=c[l+96>>2]|0;q=+g[k+4>>2]*p+ +g[j+(m<<3)+4>>2];g[e>>2]=+g[j+(m<<3)>>2]+o*p;g[e+4>>2]=q;m=b+8|0;b=c[m>>2]|0;j=c[b+12>>2]|0;if(lb[c[(c[j>>2]|0)+16>>2]&31](j,(c[b+8>>2]|0)+12|0,e)|0){n=0;i=d;return n|0}b=c[(c[m>>2]|0)+12>>2]|0;j=mb[c[(c[b>>2]|0)+12>>2]&15](b)|0;b=0;while(1){if((b|0)>=(j|0)){break}k=c[m>>2]|0;l=c[k+12>>2]|0;sb[c[(c[l>>2]|0)+20>>2]&31](l,(c[k+8>>2]|0)+12|0,e,f,h,b);if(+g[f>>2]<.004999999888241291){n=0;r=10;break}else{b=b+1|0}}if((r|0)==10){i=d;return n|0}r=c[a+12>>2]|0;c[r>>2]=(c[r>>2]|0)+1;n=1;i=d;return n|0}function Ll(a,b,d){a=a|0;b=b|0;d=d|0;var e=0,f=0,g=0,h=0,j=0,k=0,l=0,m=0,n=0,o=0,p=0,q=0,r=0,s=0,t=0,u=0,v=0,w=0,x=0,y=0,z=0,A=0,B=0,C=0,D=0,E=0,F=0,G=0,H=0,I=0,J=0,K=0,L=0,M=0,N=0,O=0,P=0,Q=0,R=0,S=0,T=0,U=0,V=0,W=0,X=0,Y=0,Z=0,_=0,$=0,aa=0,ba=0,ca=0,da=0,ea=0,fa=0,ga=0,ha=0;e=i;i=i+768|0;f=e+448|0;g=e+480|0;h=e+512|0;j=e+544|0;k=e+576|0;l=e+608|0;m=e+640|0;n=e+672|0;o=e+704|0;p=e+736|0;q=e+416|0;r=e+352|0;s=e+320|0;t=e+384|0;u=e|0;v=e+32|0;w=e+96|0;x=e+128|0;y=e+64|0;z=e+160|0;A=e+192|0;B=e+256|0;C=e+288|0;D=e+224|0;E=a;a=b;a:while(1){b=a;F=a-28|0;G=F;H=E;b:while(1){I=H;J=b-I|0;switch((J|0)/28|0|0){case 5:{K=15;break a;break};case 4:{K=14;break a;break};case 2:{K=4;break a;break};case 3:{K=6;break a;break};case 0:case 1:{K=66;break a;break};default:{}}if((J|0)<868){K=21;break a}L=(J|0)/56|0;M=H+(L*28|0)|0;do{if((J|0)>27972){N=(J|0)/112|0;O=H+(N*28|0)|0;P=H+((N+L|0)*28|0)|0;N=Ml(H,O,M,P,d)|0;if(!(tb[c[d>>2]&63](F,P)|0)){Q=N;break}R=P;c[q>>2]=c[R>>2];c[q+4>>2]=c[R+4>>2];c[q+8>>2]=c[R+8>>2];c[q+12>>2]=c[R+12>>2];c[q+16>>2]=c[R+16>>2];c[q+20>>2]=c[R+20>>2];c[q+24>>2]=c[R+24>>2];c[R>>2]=c[G>>2];c[R+4>>2]=c[G+4>>2];c[R+8>>2]=c[G+8>>2];c[R+12>>2]=c[G+12>>2];c[R+16>>2]=c[G+16>>2];c[R+20>>2]=c[G+20>>2];c[R+24>>2]=c[G+24>>2];c[G>>2]=c[q>>2];c[G+4>>2]=c[q+4>>2];c[G+8>>2]=c[q+8>>2];c[G+12>>2]=c[q+12>>2];c[G+16>>2]=c[q+16>>2];c[G+20>>2]=c[q+20>>2];c[G+24>>2]=c[q+24>>2];if(!(tb[c[d>>2]&63](P,M)|0)){Q=N+1|0;break}P=M;c[r>>2]=c[P>>2];c[r+4>>2]=c[P+4>>2];c[r+8>>2]=c[P+8>>2];c[r+12>>2]=c[P+12>>2];c[r+16>>2]=c[P+16>>2];c[r+20>>2]=c[P+20>>2];c[r+24>>2]=c[P+24>>2];c[P>>2]=c[R>>2];c[P+4>>2]=c[R+4>>2];c[P+8>>2]=c[R+8>>2];c[P+12>>2]=c[R+12>>2];c[P+16>>2]=c[R+16>>2];c[P+20>>2]=c[R+20>>2];c[P+24>>2]=c[R+24>>2];c[R>>2]=c[r>>2];c[R+4>>2]=c[r+4>>2];c[R+8>>2]=c[r+8>>2];c[R+12>>2]=c[r+12>>2];c[R+16>>2]=c[r+16>>2];c[R+20>>2]=c[r+20>>2];c[R+24>>2]=c[r+24>>2];if(!(tb[c[d>>2]&63](M,O)|0)){Q=N+2|0;break}R=O;c[s>>2]=c[R>>2];c[s+4>>2]=c[R+4>>2];c[s+8>>2]=c[R+8>>2];c[s+12>>2]=c[R+12>>2];c[s+16>>2]=c[R+16>>2];c[s+20>>2]=c[R+20>>2];c[s+24>>2]=c[R+24>>2];c[R>>2]=c[P>>2];c[R+4>>2]=c[P+4>>2];c[R+8>>2]=c[P+8>>2];c[R+12>>2]=c[P+12>>2];c[R+16>>2]=c[P+16>>2];c[R+20>>2]=c[P+20>>2];c[R+24>>2]=c[P+24>>2];c[P>>2]=c[s>>2];c[P+4>>2]=c[s+4>>2];c[P+8>>2]=c[s+8>>2];c[P+12>>2]=c[s+12>>2];c[P+16>>2]=c[s+16>>2];c[P+20>>2]=c[s+20>>2];c[P+24>>2]=c[s+24>>2];if(!(tb[c[d>>2]&63](O,H)|0)){Q=N+3|0;break}O=H;c[t>>2]=c[O>>2];c[t+4>>2]=c[O+4>>2];c[t+8>>2]=c[O+8>>2];c[t+12>>2]=c[O+12>>2];c[t+16>>2]=c[O+16>>2];c[t+20>>2]=c[O+20>>2];c[t+24>>2]=c[O+24>>2];c[O>>2]=c[R>>2];c[O+4>>2]=c[R+4>>2];c[O+8>>2]=c[R+8>>2];c[O+12>>2]=c[R+12>>2];c[O+16>>2]=c[R+16>>2];c[O+20>>2]=c[R+20>>2];c[O+24>>2]=c[R+24>>2];c[R>>2]=c[t>>2];c[R+4>>2]=c[t+4>>2];c[R+8>>2]=c[t+8>>2];c[R+12>>2]=c[t+12>>2];c[R+16>>2]=c[t+16>>2];c[R+20>>2]=c[t+20>>2];c[R+24>>2]=c[t+24>>2];Q=N+4|0}else{N=tb[c[d>>2]&63](M,H)|0;R=tb[c[d>>2]&63](F,M)|0;if(!N){if(!R){Q=0;break}N=M;c[C>>2]=c[N>>2];c[C+4>>2]=c[N+4>>2];c[C+8>>2]=c[N+8>>2];c[C+12>>2]=c[N+12>>2];c[C+16>>2]=c[N+16>>2];c[C+20>>2]=c[N+20>>2];c[C+24>>2]=c[N+24>>2];c[N>>2]=c[G>>2];c[N+4>>2]=c[G+4>>2];c[N+8>>2]=c[G+8>>2];c[N+12>>2]=c[G+12>>2];c[N+16>>2]=c[G+16>>2];c[N+20>>2]=c[G+20>>2];c[N+24>>2]=c[G+24>>2];c[G>>2]=c[C>>2];c[G+4>>2]=c[C+4>>2];c[G+8>>2]=c[C+8>>2];c[G+12>>2]=c[C+12>>2];c[G+16>>2]=c[C+16>>2];c[G+20>>2]=c[C+20>>2];c[G+24>>2]=c[C+24>>2];if(!(tb[c[d>>2]&63](M,H)|0)){Q=1;break}O=H;c[D>>2]=c[O>>2];c[D+4>>2]=c[O+4>>2];c[D+8>>2]=c[O+8>>2];c[D+12>>2]=c[O+12>>2];c[D+16>>2]=c[O+16>>2];c[D+20>>2]=c[O+20>>2];c[D+24>>2]=c[O+24>>2];c[O>>2]=c[N>>2];c[O+4>>2]=c[N+4>>2];c[O+8>>2]=c[N+8>>2];c[O+12>>2]=c[N+12>>2];c[O+16>>2]=c[N+16>>2];c[O+20>>2]=c[N+20>>2];c[O+24>>2]=c[N+24>>2];c[N>>2]=c[D>>2];c[N+4>>2]=c[D+4>>2];c[N+8>>2]=c[D+8>>2];c[N+12>>2]=c[D+12>>2];c[N+16>>2]=c[D+16>>2];c[N+20>>2]=c[D+20>>2];c[N+24>>2]=c[D+24>>2];Q=2;break}if(R){R=H;c[z>>2]=c[R>>2];c[z+4>>2]=c[R+4>>2];c[z+8>>2]=c[R+8>>2];c[z+12>>2]=c[R+12>>2];c[z+16>>2]=c[R+16>>2];c[z+20>>2]=c[R+20>>2];c[z+24>>2]=c[R+24>>2];c[R>>2]=c[G>>2];c[R+4>>2]=c[G+4>>2];c[R+8>>2]=c[G+8>>2];c[R+12>>2]=c[G+12>>2];c[R+16>>2]=c[G+16>>2];c[R+20>>2]=c[G+20>>2];c[R+24>>2]=c[G+24>>2];c[G>>2]=c[z>>2];c[G+4>>2]=c[z+4>>2];c[G+8>>2]=c[z+8>>2];c[G+12>>2]=c[z+12>>2];c[G+16>>2]=c[z+16>>2];c[G+20>>2]=c[z+20>>2];c[G+24>>2]=c[z+24>>2];Q=1;break}R=H;c[A>>2]=c[R>>2];c[A+4>>2]=c[R+4>>2];c[A+8>>2]=c[R+8>>2];c[A+12>>2]=c[R+12>>2];c[A+16>>2]=c[R+16>>2];c[A+20>>2]=c[R+20>>2];c[A+24>>2]=c[R+24>>2];N=M;c[R>>2]=c[N>>2];c[R+4>>2]=c[N+4>>2];c[R+8>>2]=c[N+8>>2];c[R+12>>2]=c[N+12>>2];c[R+16>>2]=c[N+16>>2];c[R+20>>2]=c[N+20>>2];c[R+24>>2]=c[N+24>>2];c[N>>2]=c[A>>2];c[N+4>>2]=c[A+4>>2];c[N+8>>2]=c[A+8>>2];c[N+12>>2]=c[A+12>>2];c[N+16>>2]=c[A+16>>2];c[N+20>>2]=c[A+20>>2];c[N+24>>2]=c[A+24>>2];if(!(tb[c[d>>2]&63](F,M)|0)){Q=1;break}c[B>>2]=c[N>>2];c[B+4>>2]=c[N+4>>2];c[B+8>>2]=c[N+8>>2];c[B+12>>2]=c[N+12>>2];c[B+16>>2]=c[N+16>>2];c[B+20>>2]=c[N+20>>2];c[B+24>>2]=c[N+24>>2];c[N>>2]=c[G>>2];c[N+4>>2]=c[G+4>>2];c[N+8>>2]=c[G+8>>2];c[N+12>>2]=c[G+12>>2];c[N+16>>2]=c[G+16>>2];c[N+20>>2]=c[G+20>>2];c[N+24>>2]=c[G+24>>2];c[G>>2]=c[B>>2];c[G+4>>2]=c[B+4>>2];c[G+8>>2]=c[B+8>>2];c[G+12>>2]=c[B+12>>2];c[G+16>>2]=c[B+16>>2];c[G+20>>2]=c[B+20>>2];c[G+24>>2]=c[B+24>>2];Q=2}}while(0);do{if(tb[c[d>>2]&63](H,M)|0){S=F;T=Q}else{L=F;while(1){U=L-28|0;if((H|0)==(U|0)){break}if(tb[c[d>>2]&63](U,M)|0){K=49;break}else{L=U}}if((K|0)==49){K=0;L=H;c[y>>2]=c[L>>2];c[y+4>>2]=c[L+4>>2];c[y+8>>2]=c[L+8>>2];c[y+12>>2]=c[L+12>>2];c[y+16>>2]=c[L+16>>2];c[y+20>>2]=c[L+20>>2];c[y+24>>2]=c[L+24>>2];J=U;c[L>>2]=c[J>>2];c[L+4>>2]=c[J+4>>2];c[L+8>>2]=c[J+8>>2];c[L+12>>2]=c[J+12>>2];c[L+16>>2]=c[J+16>>2];c[L+20>>2]=c[J+20>>2];c[L+24>>2]=c[J+24>>2];c[J>>2]=c[y>>2];c[J+4>>2]=c[y+4>>2];c[J+8>>2]=c[y+8>>2];c[J+12>>2]=c[y+12>>2];c[J+16>>2]=c[y+16>>2];c[J+20>>2]=c[y+20>>2];c[J+24>>2]=c[y+24>>2];S=U;T=Q+1|0;break}J=H+28|0;if(tb[c[d>>2]&63](H,F)|0){V=J}else{L=J;while(1){if((L|0)==(F|0)){K=66;break a}W=L+28|0;if(tb[c[d>>2]&63](H,L)|0){break}else{L=W}}J=L;c[x>>2]=c[J>>2];c[x+4>>2]=c[J+4>>2];c[x+8>>2]=c[J+8>>2];c[x+12>>2]=c[J+12>>2];c[x+16>>2]=c[J+16>>2];c[x+20>>2]=c[J+20>>2];c[x+24>>2]=c[J+24>>2];c[J>>2]=c[G>>2];c[J+4>>2]=c[G+4>>2];c[J+8>>2]=c[G+8>>2];c[J+12>>2]=c[G+12>>2];c[J+16>>2]=c[G+16>>2];c[J+20>>2]=c[G+20>>2];c[J+24>>2]=c[G+24>>2];c[G>>2]=c[x>>2];c[G+4>>2]=c[x+4>>2];c[G+8>>2]=c[x+8>>2];c[G+12>>2]=c[x+12>>2];c[G+16>>2]=c[x+16>>2];c[G+20>>2]=c[x+20>>2];c[G+24>>2]=c[x+24>>2];V=W}if((V|0)==(F|0)){K=66;break a}else{X=F;Y=V}while(1){J=Y;while(1){Z=J+28|0;if(tb[c[d>>2]&63](H,J)|0){_=X;break}else{J=Z}}do{_=_-28|0;}while(tb[c[d>>2]&63](H,_)|0);if(!(J>>>0<_>>>0)){H=J;continue b}N=J;c[w>>2]=c[N>>2];c[w+4>>2]=c[N+4>>2];c[w+8>>2]=c[N+8>>2];c[w+12>>2]=c[N+12>>2];c[w+16>>2]=c[N+16>>2];c[w+20>>2]=c[N+20>>2];c[w+24>>2]=c[N+24>>2];R=_;c[N>>2]=c[R>>2];c[N+4>>2]=c[R+4>>2];c[N+8>>2]=c[R+8>>2];c[N+12>>2]=c[R+12>>2];c[N+16>>2]=c[R+16>>2];c[N+20>>2]=c[R+20>>2];c[N+24>>2]=c[R+24>>2];c[R>>2]=c[w>>2];c[R+4>>2]=c[w+4>>2];c[R+8>>2]=c[w+8>>2];c[R+12>>2]=c[w+12>>2];c[R+16>>2]=c[w+16>>2];c[R+20>>2]=c[w+20>>2];c[R+24>>2]=c[w+24>>2];X=_;Y=Z}}}while(0);L=H+28|0;c:do{if(L>>>0<S>>>0){R=S;N=L;O=T;P=M;while(1){$=N;while(1){aa=$+28|0;if(tb[c[d>>2]&63]($,P)|0){$=aa}else{ba=R;break}}do{ba=ba-28|0;}while(!(tb[c[d>>2]&63](ba,P)|0));if($>>>0>ba>>>0){ca=$;da=O;ea=P;break c}J=$;c[v>>2]=c[J>>2];c[v+4>>2]=c[J+4>>2];c[v+8>>2]=c[J+8>>2];c[v+12>>2]=c[J+12>>2];c[v+16>>2]=c[J+16>>2];c[v+20>>2]=c[J+20>>2];c[v+24>>2]=c[J+24>>2];fa=ba;c[J>>2]=c[fa>>2];c[J+4>>2]=c[fa+4>>2];c[J+8>>2]=c[fa+8>>2];c[J+12>>2]=c[fa+12>>2];c[J+16>>2]=c[fa+16>>2];c[J+20>>2]=c[fa+20>>2];c[J+24>>2]=c[fa+24>>2];c[fa>>2]=c[v>>2];c[fa+4>>2]=c[v+4>>2];c[fa+8>>2]=c[v+8>>2];c[fa+12>>2]=c[v+12>>2];c[fa+16>>2]=c[v+16>>2];c[fa+20>>2]=c[v+20>>2];c[fa+24>>2]=c[v+24>>2];R=ba;N=aa;O=O+1|0;P=(P|0)==($|0)?ba:P}}else{ca=L;da=T;ea=M}}while(0);do{if((ca|0)==(ea|0)){ga=da}else{if(!(tb[c[d>>2]&63](ea,ca)|0)){ga=da;break}M=ca;c[u>>2]=c[M>>2];c[u+4>>2]=c[M+4>>2];c[u+8>>2]=c[M+8>>2];c[u+12>>2]=c[M+12>>2];c[u+16>>2]=c[M+16>>2];c[u+20>>2]=c[M+20>>2];c[u+24>>2]=c[M+24>>2];L=ea;c[M>>2]=c[L>>2];c[M+4>>2]=c[L+4>>2];c[M+8>>2]=c[L+8>>2];c[M+12>>2]=c[L+12>>2];c[M+16>>2]=c[L+16>>2];c[M+20>>2]=c[L+20>>2];c[M+24>>2]=c[L+24>>2];c[L>>2]=c[u>>2];c[L+4>>2]=c[u+4>>2];c[L+8>>2]=c[u+8>>2];c[L+12>>2]=c[u+12>>2];c[L+16>>2]=c[u+16>>2];c[L+20>>2]=c[u+20>>2];c[L+24>>2]=c[u+24>>2];ga=da+1|0}}while(0);if((ga|0)==0){ha=Ol(H,ca,d)|0;L=ca+28|0;if(Ol(L,a,d)|0){K=61;break}if(ha){H=L;continue}}L=ca;if((L-I|0)>=(b-L|0)){K=65;break}Ll(H,ca,d);H=ca+28|0}if((K|0)==61){K=0;if(ha){K=66;break}else{E=H;a=ca;continue}}else if((K|0)==65){K=0;Ll(ca+28|0,a,d);E=H;a=ca;continue}}if((K|0)==4){if(!(tb[c[d>>2]&63](F,H)|0)){i=e;return}ca=p;p=H;c[ca>>2]=c[p>>2];c[ca+4>>2]=c[p+4>>2];c[ca+8>>2]=c[p+8>>2];c[ca+12>>2]=c[p+12>>2];c[ca+16>>2]=c[p+16>>2];c[ca+20>>2]=c[p+20>>2];c[ca+24>>2]=c[p+24>>2];c[p>>2]=c[G>>2];c[p+4>>2]=c[G+4>>2];c[p+8>>2]=c[G+8>>2];c[p+12>>2]=c[G+12>>2];c[p+16>>2]=c[G+16>>2];c[p+20>>2]=c[G+20>>2];c[p+24>>2]=c[G+24>>2];c[G>>2]=c[ca>>2];c[G+4>>2]=c[ca+4>>2];c[G+8>>2]=c[ca+8>>2];c[G+12>>2]=c[ca+12>>2];c[G+16>>2]=c[ca+16>>2];c[G+20>>2]=c[ca+20>>2];c[G+24>>2]=c[ca+24>>2];i=e;return}else if((K|0)==6){ca=H+28|0;p=tb[c[d>>2]&63](ca,H)|0;E=tb[c[d>>2]&63](F,ca)|0;if(!p){if(!E){i=e;return}p=o;o=ca;c[p>>2]=c[o>>2];c[p+4>>2]=c[o+4>>2];c[p+8>>2]=c[o+8>>2];c[p+12>>2]=c[o+12>>2];c[p+16>>2]=c[o+16>>2];c[p+20>>2]=c[o+20>>2];c[p+24>>2]=c[o+24>>2];c[o>>2]=c[G>>2];c[o+4>>2]=c[G+4>>2];c[o+8>>2]=c[G+8>>2];c[o+12>>2]=c[G+12>>2];c[o+16>>2]=c[G+16>>2];c[o+20>>2]=c[G+20>>2];c[o+24>>2]=c[G+24>>2];c[G>>2]=c[p>>2];c[G+4>>2]=c[p+4>>2];c[G+8>>2]=c[p+8>>2];c[G+12>>2]=c[p+12>>2];c[G+16>>2]=c[p+16>>2];c[G+20>>2]=c[p+20>>2];c[G+24>>2]=c[p+24>>2];if(!(tb[c[d>>2]&63](ca,H)|0)){i=e;return}p=m;m=H;c[p>>2]=c[m>>2];c[p+4>>2]=c[m+4>>2];c[p+8>>2]=c[m+8>>2];c[p+12>>2]=c[m+12>>2];c[p+16>>2]=c[m+16>>2];c[p+20>>2]=c[m+20>>2];c[p+24>>2]=c[m+24>>2];c[m>>2]=c[o>>2];c[m+4>>2]=c[o+4>>2];c[m+8>>2]=c[o+8>>2];c[m+12>>2]=c[o+12>>2];c[m+16>>2]=c[o+16>>2];c[m+20>>2]=c[o+20>>2];c[m+24>>2]=c[o+24>>2];c[o>>2]=c[p>>2];c[o+4>>2]=c[p+4>>2];c[o+8>>2]=c[p+8>>2];c[o+12>>2]=c[p+12>>2];c[o+16>>2]=c[p+16>>2];c[o+20>>2]=c[p+20>>2];c[o+24>>2]=c[p+24>>2];i=e;return}if(E){E=k;k=H;c[E>>2]=c[k>>2];c[E+4>>2]=c[k+4>>2];c[E+8>>2]=c[k+8>>2];c[E+12>>2]=c[k+12>>2];c[E+16>>2]=c[k+16>>2];c[E+20>>2]=c[k+20>>2];c[E+24>>2]=c[k+24>>2];c[k>>2]=c[G>>2];c[k+4>>2]=c[G+4>>2];c[k+8>>2]=c[G+8>>2];c[k+12>>2]=c[G+12>>2];c[k+16>>2]=c[G+16>>2];c[k+20>>2]=c[G+20>>2];c[k+24>>2]=c[G+24>>2];c[G>>2]=c[E>>2];c[G+4>>2]=c[E+4>>2];c[G+8>>2]=c[E+8>>2];c[G+12>>2]=c[E+12>>2];c[G+16>>2]=c[E+16>>2];c[G+20>>2]=c[E+20>>2];c[G+24>>2]=c[E+24>>2];i=e;return}E=l;l=H;c[E>>2]=c[l>>2];c[E+4>>2]=c[l+4>>2];c[E+8>>2]=c[l+8>>2];c[E+12>>2]=c[l+12>>2];c[E+16>>2]=c[l+16>>2];c[E+20>>2]=c[l+20>>2];c[E+24>>2]=c[l+24>>2];k=ca;c[l>>2]=c[k>>2];c[l+4>>2]=c[k+4>>2];c[l+8>>2]=c[k+8>>2];c[l+12>>2]=c[k+12>>2];c[l+16>>2]=c[k+16>>2];c[l+20>>2]=c[k+20>>2];c[l+24>>2]=c[k+24>>2];c[k>>2]=c[E>>2];c[k+4>>2]=c[E+4>>2];c[k+8>>2]=c[E+8>>2];c[k+12>>2]=c[E+12>>2];c[k+16>>2]=c[E+16>>2];c[k+20>>2]=c[E+20>>2];c[k+24>>2]=c[E+24>>2];if(!(tb[c[d>>2]&63](F,ca)|0)){i=e;return}ca=n;c[ca>>2]=c[k>>2];c[ca+4>>2]=c[k+4>>2];c[ca+8>>2]=c[k+8>>2];c[ca+12>>2]=c[k+12>>2];c[ca+16>>2]=c[k+16>>2];c[ca+20>>2]=c[k+20>>2];c[ca+24>>2]=c[k+24>>2];c[k>>2]=c[G>>2];c[k+4>>2]=c[G+4>>2];c[k+8>>2]=c[G+8>>2];c[k+12>>2]=c[G+12>>2];c[k+16>>2]=c[G+16>>2];c[k+20>>2]=c[G+20>>2];c[k+24>>2]=c[G+24>>2];c[G>>2]=c[ca>>2];c[G+4>>2]=c[ca+4>>2];c[G+8>>2]=c[ca+8>>2];c[G+12>>2]=c[ca+12>>2];c[G+16>>2]=c[ca+16>>2];c[G+20>>2]=c[ca+20>>2];c[G+24>>2]=c[ca+24>>2];i=e;return}else if((K|0)==14){Ml(H,H+28|0,H+56|0,F,d)|0;i=e;return}else if((K|0)==15){ca=H+28|0;k=H+56|0;n=H+84|0;Ml(H,ca,k,n,d)|0;if(!(tb[c[d>>2]&63](F,n)|0)){i=e;return}F=j;j=n;c[F>>2]=c[j>>2];c[F+4>>2]=c[j+4>>2];c[F+8>>2]=c[j+8>>2];c[F+12>>2]=c[j+12>>2];c[F+16>>2]=c[j+16>>2];c[F+20>>2]=c[j+20>>2];c[F+24>>2]=c[j+24>>2];c[j>>2]=c[G>>2];c[j+4>>2]=c[G+4>>2];c[j+8>>2]=c[G+8>>2];c[j+12>>2]=c[G+12>>2];c[j+16>>2]=c[G+16>>2];c[j+20>>2]=c[G+20>>2];c[j+24>>2]=c[G+24>>2];c[G>>2]=c[F>>2];c[G+4>>2]=c[F+4>>2];c[G+8>>2]=c[F+8>>2];c[G+12>>2]=c[F+12>>2];c[G+16>>2]=c[F+16>>2];c[G+20>>2]=c[F+20>>2];c[G+24>>2]=c[F+24>>2];if(!(tb[c[d>>2]&63](n,k)|0)){i=e;return}n=g;g=k;c[n>>2]=c[g>>2];c[n+4>>2]=c[g+4>>2];c[n+8>>2]=c[g+8>>2];c[n+12>>2]=c[g+12>>2];c[n+16>>2]=c[g+16>>2];c[n+20>>2]=c[g+20>>2];c[n+24>>2]=c[g+24>>2];c[g>>2]=c[j>>2];c[g+4>>2]=c[j+4>>2];c[g+8>>2]=c[j+8>>2];c[g+12>>2]=c[j+12>>2];c[g+16>>2]=c[j+16>>2];c[g+20>>2]=c[j+20>>2];c[g+24>>2]=c[j+24>>2];c[j>>2]=c[n>>2];c[j+4>>2]=c[n+4>>2];c[j+8>>2]=c[n+8>>2];c[j+12>>2]=c[n+12>>2];c[j+16>>2]=c[n+16>>2];c[j+20>>2]=c[n+20>>2];c[j+24>>2]=c[n+24>>2];if(!(tb[c[d>>2]&63](k,ca)|0)){i=e;return}k=f;f=ca;c[k>>2]=c[f>>2];c[k+4>>2]=c[f+4>>2];c[k+8>>2]=c[f+8>>2];c[k+12>>2]=c[f+12>>2];c[k+16>>2]=c[f+16>>2];c[k+20>>2]=c[f+20>>2];c[k+24>>2]=c[f+24>>2];c[f>>2]=c[g>>2];c[f+4>>2]=c[g+4>>2];c[f+8>>2]=c[g+8>>2];c[f+12>>2]=c[g+12>>2];c[f+16>>2]=c[g+16>>2];c[f+20>>2]=c[g+20>>2];c[f+24>>2]=c[g+24>>2];c[g>>2]=c[k>>2];c[g+4>>2]=c[k+4>>2];c[g+8>>2]=c[k+8>>2];c[g+12>>2]=c[k+12>>2];c[g+16>>2]=c[k+16>>2];c[g+20>>2]=c[k+20>>2];c[g+24>>2]=c[k+24>>2];if(!(tb[c[d>>2]&63](ca,H)|0)){i=e;return}ca=h;h=H;c[ca>>2]=c[h>>2];c[ca+4>>2]=c[h+4>>2];c[ca+8>>2]=c[h+8>>2];c[ca+12>>2]=c[h+12>>2];c[ca+16>>2]=c[h+16>>2];c[ca+20>>2]=c[h+20>>2];c[ca+24>>2]=c[h+24>>2];c[h>>2]=c[f>>2];c[h+4>>2]=c[f+4>>2];c[h+8>>2]=c[f+8>>2];c[h+12>>2]=c[f+12>>2];c[h+16>>2]=c[f+16>>2];c[h+20>>2]=c[f+20>>2];c[h+24>>2]=c[f+24>>2];c[f>>2]=c[ca>>2];c[f+4>>2]=c[ca+4>>2];c[f+8>>2]=c[ca+8>>2];c[f+12>>2]=c[ca+12>>2];c[f+16>>2]=c[ca+16>>2];c[f+20>>2]=c[ca+20>>2];c[f+24>>2]=c[ca+24>>2];i=e;return}else if((K|0)==21){Nl(H,a,d);i=e;return}else if((K|0)==66){i=e;return}}function Ml(a,b,d,e,f){a=a|0;b=b|0;d=d|0;e=e|0;f=f|0;var g=0,h=0,j=0,k=0,l=0,m=0,n=0,o=0,p=0,q=0,r=0,s=0,t=0,u=0,v=0,w=0;g=i;i=i+256|0;h=g|0;j=g+32|0;k=g+64|0;l=g+96|0;m=g+128|0;n=g+160|0;o=g+192|0;p=g+224|0;q=tb[c[f>>2]&63](b,a)|0;r=tb[c[f>>2]&63](d,b)|0;do{if(q){if(r){s=l;t=a;c[s>>2]=c[t>>2];c[s+4>>2]=c[t+4>>2];c[s+8>>2]=c[t+8>>2];c[s+12>>2]=c[t+12>>2];c[s+16>>2]=c[t+16>>2];c[s+20>>2]=c[t+20>>2];c[s+24>>2]=c[t+24>>2];u=d;c[t>>2]=c[u>>2];c[t+4>>2]=c[u+4>>2];c[t+8>>2]=c[u+8>>2];c[t+12>>2]=c[u+12>>2];c[t+16>>2]=c[u+16>>2];c[t+20>>2]=c[u+20>>2];c[t+24>>2]=c[u+24>>2];c[u>>2]=c[s>>2];c[u+4>>2]=c[s+4>>2];c[u+8>>2]=c[s+8>>2];c[u+12>>2]=c[s+12>>2];c[u+16>>2]=c[s+16>>2];c[u+20>>2]=c[s+20>>2];c[u+24>>2]=c[s+24>>2];v=1;break}s=m;u=a;c[s>>2]=c[u>>2];c[s+4>>2]=c[u+4>>2];c[s+8>>2]=c[u+8>>2];c[s+12>>2]=c[u+12>>2];c[s+16>>2]=c[u+16>>2];c[s+20>>2]=c[u+20>>2];c[s+24>>2]=c[u+24>>2];t=b;c[u>>2]=c[t>>2];c[u+4>>2]=c[t+4>>2];c[u+8>>2]=c[t+8>>2];c[u+12>>2]=c[t+12>>2];c[u+16>>2]=c[t+16>>2];c[u+20>>2]=c[t+20>>2];c[u+24>>2]=c[t+24>>2];c[t>>2]=c[s>>2];c[t+4>>2]=c[s+4>>2];c[t+8>>2]=c[s+8>>2];c[t+12>>2]=c[s+12>>2];c[t+16>>2]=c[s+16>>2];c[t+20>>2]=c[s+20>>2];c[t+24>>2]=c[s+24>>2];if(!(tb[c[f>>2]&63](d,b)|0)){v=1;break}s=o;c[s>>2]=c[t>>2];c[s+4>>2]=c[t+4>>2];c[s+8>>2]=c[t+8>>2];c[s+12>>2]=c[t+12>>2];c[s+16>>2]=c[t+16>>2];c[s+20>>2]=c[t+20>>2];c[s+24>>2]=c[t+24>>2];u=d;c[t>>2]=c[u>>2];c[t+4>>2]=c[u+4>>2];c[t+8>>2]=c[u+8>>2];c[t+12>>2]=c[u+12>>2];c[t+16>>2]=c[u+16>>2];c[t+20>>2]=c[u+20>>2];c[t+24>>2]=c[u+24>>2];c[u>>2]=c[s>>2];c[u+4>>2]=c[s+4>>2];c[u+8>>2]=c[s+8>>2];c[u+12>>2]=c[s+12>>2];c[u+16>>2]=c[s+16>>2];c[u+20>>2]=c[s+20>>2];c[u+24>>2]=c[s+24>>2];v=2}else{if(!r){v=0;break}s=p;u=b;c[s>>2]=c[u>>2];c[s+4>>2]=c[u+4>>2];c[s+8>>2]=c[u+8>>2];c[s+12>>2]=c[u+12>>2];c[s+16>>2]=c[u+16>>2];c[s+20>>2]=c[u+20>>2];c[s+24>>2]=c[u+24>>2];t=d;c[u>>2]=c[t>>2];c[u+4>>2]=c[t+4>>2];c[u+8>>2]=c[t+8>>2];c[u+12>>2]=c[t+12>>2];c[u+16>>2]=c[t+16>>2];c[u+20>>2]=c[t+20>>2];c[u+24>>2]=c[t+24>>2];c[t>>2]=c[s>>2];c[t+4>>2]=c[s+4>>2];c[t+8>>2]=c[s+8>>2];c[t+12>>2]=c[s+12>>2];c[t+16>>2]=c[s+16>>2];c[t+20>>2]=c[s+20>>2];c[t+24>>2]=c[s+24>>2];if(!(tb[c[f>>2]&63](b,a)|0)){v=1;break}s=n;t=a;c[s>>2]=c[t>>2];c[s+4>>2]=c[t+4>>2];c[s+8>>2]=c[t+8>>2];c[s+12>>2]=c[t+12>>2];c[s+16>>2]=c[t+16>>2];c[s+20>>2]=c[t+20>>2];c[s+24>>2]=c[t+24>>2];c[t>>2]=c[u>>2];c[t+4>>2]=c[u+4>>2];c[t+8>>2]=c[u+8>>2];c[t+12>>2]=c[u+12>>2];c[t+16>>2]=c[u+16>>2];c[t+20>>2]=c[u+20>>2];c[t+24>>2]=c[u+24>>2];c[u>>2]=c[s>>2];c[u+4>>2]=c[s+4>>2];c[u+8>>2]=c[s+8>>2];c[u+12>>2]=c[s+12>>2];c[u+16>>2]=c[s+16>>2];c[u+20>>2]=c[s+20>>2];c[u+24>>2]=c[s+24>>2];v=2}}while(0);if(!(tb[c[f>>2]&63](e,d)|0)){w=v;i=g;return w|0}n=k;k=d;c[n>>2]=c[k>>2];c[n+4>>2]=c[k+4>>2];c[n+8>>2]=c[k+8>>2];c[n+12>>2]=c[k+12>>2];c[n+16>>2]=c[k+16>>2];c[n+20>>2]=c[k+20>>2];c[n+24>>2]=c[k+24>>2];p=e;c[k>>2]=c[p>>2];c[k+4>>2]=c[p+4>>2];c[k+8>>2]=c[p+8>>2];c[k+12>>2]=c[p+12>>2];c[k+16>>2]=c[p+16>>2];c[k+20>>2]=c[p+20>>2];c[k+24>>2]=c[p+24>>2];c[p>>2]=c[n>>2];c[p+4>>2]=c[n+4>>2];c[p+8>>2]=c[n+8>>2];c[p+12>>2]=c[n+12>>2];c[p+16>>2]=c[n+16>>2];c[p+20>>2]=c[n+20>>2];c[p+24>>2]=c[n+24>>2];if(!(tb[c[f>>2]&63](d,b)|0)){w=v+1|0;i=g;return w|0}d=j;j=b;c[d>>2]=c[j>>2];c[d+4>>2]=c[j+4>>2];c[d+8>>2]=c[j+8>>2];c[d+12>>2]=c[j+12>>2];c[d+16>>2]=c[j+16>>2];c[d+20>>2]=c[j+20>>2];c[d+24>>2]=c[j+24>>2];c[j>>2]=c[k>>2];c[j+4>>2]=c[k+4>>2];c[j+8>>2]=c[k+8>>2];c[j+12>>2]=c[k+12>>2];c[j+16>>2]=c[k+16>>2];c[j+20>>2]=c[k+20>>2];c[j+24>>2]=c[k+24>>2];c[k>>2]=c[d>>2];c[k+4>>2]=c[d+4>>2];c[k+8>>2]=c[d+8>>2];c[k+12>>2]=c[d+12>>2];c[k+16>>2]=c[d+16>>2];c[k+20>>2]=c[d+20>>2];c[k+24>>2]=c[d+24>>2];if(!(tb[c[f>>2]&63](b,a)|0)){w=v+2|0;i=g;return w|0}b=h;h=a;c[b>>2]=c[h>>2];c[b+4>>2]=c[h+4>>2];c[b+8>>2]=c[h+8>>2];c[b+12>>2]=c[h+12>>2];c[b+16>>2]=c[h+16>>2];c[b+20>>2]=c[h+20>>2];c[b+24>>2]=c[h+24>>2];c[h>>2]=c[j>>2];c[h+4>>2]=c[j+4>>2];c[h+8>>2]=c[j+8>>2];c[h+12>>2]=c[j+12>>2];c[h+16>>2]=c[j+16>>2];c[h+20>>2]=c[j+20>>2];c[h+24>>2]=c[j+24>>2];c[j>>2]=c[b>>2];c[j+4>>2]=c[b+4>>2];c[j+8>>2]=c[b+8>>2];c[j+12>>2]=c[b+12>>2];c[j+16>>2]=c[b+16>>2];c[j+20>>2]=c[b+20>>2];c[j+24>>2]=c[b+24>>2];w=v+3|0;i=g;return w|0}function Nl(a,b,d){a=a|0;b=b|0;d=d|0;var e=0,f=0,g=0,h=0,j=0,k=0,l=0,m=0,n=0,o=0,p=0,q=0,r=0,s=0,t=0;e=i;i=i+192|0;f=e|0;g=e+32|0;h=e+64|0;j=e+96|0;k=e+128|0;l=e+160|0;m=a+56|0;n=a+28|0;o=tb[c[d>>2]&63](n,a)|0;p=tb[c[d>>2]&63](m,n)|0;do{if(o){if(p){q=f;r=a;c[q>>2]=c[r>>2];c[q+4>>2]=c[r+4>>2];c[q+8>>2]=c[r+8>>2];c[q+12>>2]=c[r+12>>2];c[q+16>>2]=c[r+16>>2];c[q+20>>2]=c[r+20>>2];c[q+24>>2]=c[r+24>>2];s=m;c[r>>2]=c[s>>2];c[r+4>>2]=c[s+4>>2];c[r+8>>2]=c[s+8>>2];c[r+12>>2]=c[s+12>>2];c[r+16>>2]=c[s+16>>2];c[r+20>>2]=c[s+20>>2];c[r+24>>2]=c[s+24>>2];c[s>>2]=c[q>>2];c[s+4>>2]=c[q+4>>2];c[s+8>>2]=c[q+8>>2];c[s+12>>2]=c[q+12>>2];c[s+16>>2]=c[q+16>>2];c[s+20>>2]=c[q+20>>2];c[s+24>>2]=c[q+24>>2];break}q=g;s=a;c[q>>2]=c[s>>2];c[q+4>>2]=c[s+4>>2];c[q+8>>2]=c[s+8>>2];c[q+12>>2]=c[s+12>>2];c[q+16>>2]=c[s+16>>2];c[q+20>>2]=c[s+20>>2];c[q+24>>2]=c[s+24>>2];r=n;c[s>>2]=c[r>>2];c[s+4>>2]=c[r+4>>2];c[s+8>>2]=c[r+8>>2];c[s+12>>2]=c[r+12>>2];c[s+16>>2]=c[r+16>>2];c[s+20>>2]=c[r+20>>2];c[s+24>>2]=c[r+24>>2];c[r>>2]=c[q>>2];c[r+4>>2]=c[q+4>>2];c[r+8>>2]=c[q+8>>2];c[r+12>>2]=c[q+12>>2];c[r+16>>2]=c[q+16>>2];c[r+20>>2]=c[q+20>>2];c[r+24>>2]=c[q+24>>2];if(!(tb[c[d>>2]&63](m,n)|0)){break}q=j;c[q>>2]=c[r>>2];c[q+4>>2]=c[r+4>>2];c[q+8>>2]=c[r+8>>2];c[q+12>>2]=c[r+12>>2];c[q+16>>2]=c[r+16>>2];c[q+20>>2]=c[r+20>>2];c[q+24>>2]=c[r+24>>2];s=m;c[r>>2]=c[s>>2];c[r+4>>2]=c[s+4>>2];c[r+8>>2]=c[s+8>>2];c[r+12>>2]=c[s+12>>2];c[r+16>>2]=c[s+16>>2];c[r+20>>2]=c[s+20>>2];c[r+24>>2]=c[s+24>>2];c[s>>2]=c[q>>2];c[s+4>>2]=c[q+4>>2];c[s+8>>2]=c[q+8>>2];c[s+12>>2]=c[q+12>>2];c[s+16>>2]=c[q+16>>2];c[s+20>>2]=c[q+20>>2];c[s+24>>2]=c[q+24>>2];}else{if(!p){break}q=k;s=n;c[q>>2]=c[s>>2];c[q+4>>2]=c[s+4>>2];c[q+8>>2]=c[s+8>>2];c[q+12>>2]=c[s+12>>2];c[q+16>>2]=c[s+16>>2];c[q+20>>2]=c[s+20>>2];c[q+24>>2]=c[s+24>>2];r=m;c[s>>2]=c[r>>2];c[s+4>>2]=c[r+4>>2];c[s+8>>2]=c[r+8>>2];c[s+12>>2]=c[r+12>>2];c[s+16>>2]=c[r+16>>2];c[s+20>>2]=c[r+20>>2];c[s+24>>2]=c[r+24>>2];c[r>>2]=c[q>>2];c[r+4>>2]=c[q+4>>2];c[r+8>>2]=c[q+8>>2];c[r+12>>2]=c[q+12>>2];c[r+16>>2]=c[q+16>>2];c[r+20>>2]=c[q+20>>2];c[r+24>>2]=c[q+24>>2];if(!(tb[c[d>>2]&63](n,a)|0)){break}q=h;r=a;c[q>>2]=c[r>>2];c[q+4>>2]=c[r+4>>2];c[q+8>>2]=c[r+8>>2];c[q+12>>2]=c[r+12>>2];c[q+16>>2]=c[r+16>>2];c[q+20>>2]=c[r+20>>2];c[q+24>>2]=c[r+24>>2];c[r>>2]=c[s>>2];c[r+4>>2]=c[s+4>>2];c[r+8>>2]=c[s+8>>2];c[r+12>>2]=c[s+12>>2];c[r+16>>2]=c[s+16>>2];c[r+20>>2]=c[s+20>>2];c[r+24>>2]=c[s+24>>2];c[s>>2]=c[q>>2];c[s+4>>2]=c[q+4>>2];c[s+8>>2]=c[q+8>>2];c[s+12>>2]=c[q+12>>2];c[s+16>>2]=c[q+16>>2];c[s+20>>2]=c[q+20>>2];c[s+24>>2]=c[q+24>>2];}}while(0);h=a+84|0;if((h|0)==(b|0)){i=e;return}n=l;k=m;m=h;while(1){if(tb[c[d>>2]&63](m,k)|0){h=m;c[n>>2]=c[h>>2];c[n+4>>2]=c[h+4>>2];c[n+8>>2]=c[h+8>>2];c[n+12>>2]=c[h+12>>2];c[n+16>>2]=c[h+16>>2];c[n+20>>2]=c[h+20>>2];c[n+24>>2]=c[h+24>>2];h=k;p=m;while(1){j=p;t=h;c[j>>2]=c[t>>2];c[j+4>>2]=c[t+4>>2];c[j+8>>2]=c[t+8>>2];c[j+12>>2]=c[t+12>>2];c[j+16>>2]=c[t+16>>2];c[j+20>>2]=c[t+20>>2];c[j+24>>2]=c[t+24>>2];if((h|0)==(a|0)){break}j=h-28|0;if(tb[c[d>>2]&63](l,j)|0){p=h;h=j}else{break}}c[t>>2]=c[n>>2];c[t+4>>2]=c[n+4>>2];c[t+8>>2]=c[n+8>>2];c[t+12>>2]=c[n+12>>2];c[t+16>>2]=c[n+16>>2];c[t+20>>2]=c[n+20>>2];c[t+24>>2]=c[n+24>>2]}h=m+28|0;if((h|0)==(b|0)){break}else{k=m;m=h}}i=e;return}function Ol(a,b,d){a=a|0;b=b|0;d=d|0;var e=0,f=0,g=0,h=0,j=0,k=0,l=0,m=0,n=0,o=0,p=0,q=0,r=0,s=0,t=0,u=0,v=0,w=0,x=0,y=0,z=0,A=0,B=0;e=i;i=i+512|0;f=e|0;g=e+32|0;h=e+64|0;j=e+96|0;k=e+128|0;l=e+288|0;m=e+320|0;n=e+352|0;o=e+384|0;p=e+416|0;q=e+480|0;switch((b-a|0)/28|0|0){case 5:{r=a+28|0;s=a+56|0;t=a+84|0;u=b-28|0;Ml(a,r,s,t,d)|0;if(!(tb[c[d>>2]&63](u,t)|0)){v=1;i=e;return v|0}w=e+256|0;x=t;c[w>>2]=c[x>>2];c[w+4>>2]=c[x+4>>2];c[w+8>>2]=c[x+8>>2];c[w+12>>2]=c[x+12>>2];c[w+16>>2]=c[x+16>>2];c[w+20>>2]=c[x+20>>2];c[w+24>>2]=c[x+24>>2];y=u;c[x>>2]=c[y>>2];c[x+4>>2]=c[y+4>>2];c[x+8>>2]=c[y+8>>2];c[x+12>>2]=c[y+12>>2];c[x+16>>2]=c[y+16>>2];c[x+20>>2]=c[y+20>>2];c[x+24>>2]=c[y+24>>2];c[y>>2]=c[w>>2];c[y+4>>2]=c[w+4>>2];c[y+8>>2]=c[w+8>>2];c[y+12>>2]=c[w+12>>2];c[y+16>>2]=c[w+16>>2];c[y+20>>2]=c[w+20>>2];c[y+24>>2]=c[w+24>>2];if(!(tb[c[d>>2]&63](t,s)|0)){v=1;i=e;return v|0}t=e+192|0;w=s;c[t>>2]=c[w>>2];c[t+4>>2]=c[w+4>>2];c[t+8>>2]=c[w+8>>2];c[t+12>>2]=c[w+12>>2];c[t+16>>2]=c[w+16>>2];c[t+20>>2]=c[w+20>>2];c[t+24>>2]=c[w+24>>2];c[w>>2]=c[x>>2];c[w+4>>2]=c[x+4>>2];c[w+8>>2]=c[x+8>>2];c[w+12>>2]=c[x+12>>2];c[w+16>>2]=c[x+16>>2];c[w+20>>2]=c[x+20>>2];c[w+24>>2]=c[x+24>>2];c[x>>2]=c[t>>2];c[x+4>>2]=c[t+4>>2];c[x+8>>2]=c[t+8>>2];c[x+12>>2]=c[t+12>>2];c[x+16>>2]=c[t+16>>2];c[x+20>>2]=c[t+20>>2];c[x+24>>2]=c[t+24>>2];if(!(tb[c[d>>2]&63](s,r)|0)){v=1;i=e;return v|0}s=e+160|0;t=r;c[s>>2]=c[t>>2];c[s+4>>2]=c[t+4>>2];c[s+8>>2]=c[t+8>>2];c[s+12>>2]=c[t+12>>2];c[s+16>>2]=c[t+16>>2];c[s+20>>2]=c[t+20>>2];c[s+24>>2]=c[t+24>>2];c[t>>2]=c[w>>2];c[t+4>>2]=c[w+4>>2];c[t+8>>2]=c[w+8>>2];c[t+12>>2]=c[w+12>>2];c[t+16>>2]=c[w+16>>2];c[t+20>>2]=c[w+20>>2];c[t+24>>2]=c[w+24>>2];c[w>>2]=c[s>>2];c[w+4>>2]=c[s+4>>2];c[w+8>>2]=c[s+8>>2];c[w+12>>2]=c[s+12>>2];c[w+16>>2]=c[s+16>>2];c[w+20>>2]=c[s+20>>2];c[w+24>>2]=c[s+24>>2];if(!(tb[c[d>>2]&63](r,a)|0)){v=1;i=e;return v|0}r=e+224|0;s=a;c[r>>2]=c[s>>2];c[r+4>>2]=c[s+4>>2];c[r+8>>2]=c[s+8>>2];c[r+12>>2]=c[s+12>>2];c[r+16>>2]=c[s+16>>2];c[r+20>>2]=c[s+20>>2];c[r+24>>2]=c[s+24>>2];c[s>>2]=c[t>>2];c[s+4>>2]=c[t+4>>2];c[s+8>>2]=c[t+8>>2];c[s+12>>2]=c[t+12>>2];c[s+16>>2]=c[t+16>>2];c[s+20>>2]=c[t+20>>2];c[s+24>>2]=c[t+24>>2];c[t>>2]=c[r>>2];c[t+4>>2]=c[r+4>>2];c[t+8>>2]=c[r+8>>2];c[t+12>>2]=c[r+12>>2];c[t+16>>2]=c[r+16>>2];c[t+20>>2]=c[r+20>>2];c[t+24>>2]=c[r+24>>2];v=1;i=e;return v|0};case 2:{r=b-28|0;if(!(tb[c[d>>2]&63](r,a)|0)){v=1;i=e;return v|0}t=e+448|0;s=a;c[t>>2]=c[s>>2];c[t+4>>2]=c[s+4>>2];c[t+8>>2]=c[s+8>>2];c[t+12>>2]=c[s+12>>2];c[t+16>>2]=c[s+16>>2];c[t+20>>2]=c[s+20>>2];c[t+24>>2]=c[s+24>>2];w=r;c[s>>2]=c[w>>2];c[s+4>>2]=c[w+4>>2];c[s+8>>2]=c[w+8>>2];c[s+12>>2]=c[w+12>>2];c[s+16>>2]=c[w+16>>2];c[s+20>>2]=c[w+20>>2];c[s+24>>2]=c[w+24>>2];c[w>>2]=c[t>>2];c[w+4>>2]=c[t+4>>2];c[w+8>>2]=c[t+8>>2];c[w+12>>2]=c[t+12>>2];c[w+16>>2]=c[t+16>>2];c[w+20>>2]=c[t+20>>2];c[w+24>>2]=c[t+24>>2];v=1;i=e;return v|0};case 0:case 1:{v=1;i=e;return v|0};case 4:{Ml(a,a+28|0,a+56|0,b-28|0,d)|0;v=1;i=e;return v|0};case 3:{t=a+28|0;w=b-28|0;s=tb[c[d>>2]&63](t,a)|0;r=tb[c[d>>2]&63](w,t)|0;if(!s){if(!r){v=1;i=e;return v|0}s=p;p=t;c[s>>2]=c[p>>2];c[s+4>>2]=c[p+4>>2];c[s+8>>2]=c[p+8>>2];c[s+12>>2]=c[p+12>>2];c[s+16>>2]=c[p+16>>2];c[s+20>>2]=c[p+20>>2];c[s+24>>2]=c[p+24>>2];x=w;c[p>>2]=c[x>>2];c[p+4>>2]=c[x+4>>2];c[p+8>>2]=c[x+8>>2];c[p+12>>2]=c[x+12>>2];c[p+16>>2]=c[x+16>>2];c[p+20>>2]=c[x+20>>2];c[p+24>>2]=c[x+24>>2];c[x>>2]=c[s>>2];c[x+4>>2]=c[s+4>>2];c[x+8>>2]=c[s+8>>2];c[x+12>>2]=c[s+12>>2];c[x+16>>2]=c[s+16>>2];c[x+20>>2]=c[s+20>>2];c[x+24>>2]=c[s+24>>2];if(!(tb[c[d>>2]&63](t,a)|0)){v=1;i=e;return v|0}s=n;n=a;c[s>>2]=c[n>>2];c[s+4>>2]=c[n+4>>2];c[s+8>>2]=c[n+8>>2];c[s+12>>2]=c[n+12>>2];c[s+16>>2]=c[n+16>>2];c[s+20>>2]=c[n+20>>2];c[s+24>>2]=c[n+24>>2];c[n>>2]=c[p>>2];c[n+4>>2]=c[p+4>>2];c[n+8>>2]=c[p+8>>2];c[n+12>>2]=c[p+12>>2];c[n+16>>2]=c[p+16>>2];c[n+20>>2]=c[p+20>>2];c[n+24>>2]=c[p+24>>2];c[p>>2]=c[s>>2];c[p+4>>2]=c[s+4>>2];c[p+8>>2]=c[s+8>>2];c[p+12>>2]=c[s+12>>2];c[p+16>>2]=c[s+16>>2];c[p+20>>2]=c[s+20>>2];c[p+24>>2]=c[s+24>>2];v=1;i=e;return v|0}if(r){r=l;l=a;c[r>>2]=c[l>>2];c[r+4>>2]=c[l+4>>2];c[r+8>>2]=c[l+8>>2];c[r+12>>2]=c[l+12>>2];c[r+16>>2]=c[l+16>>2];c[r+20>>2]=c[l+20>>2];c[r+24>>2]=c[l+24>>2];s=w;c[l>>2]=c[s>>2];c[l+4>>2]=c[s+4>>2];c[l+8>>2]=c[s+8>>2];c[l+12>>2]=c[s+12>>2];c[l+16>>2]=c[s+16>>2];c[l+20>>2]=c[s+20>>2];c[l+24>>2]=c[s+24>>2];c[s>>2]=c[r>>2];c[s+4>>2]=c[r+4>>2];c[s+8>>2]=c[r+8>>2];c[s+12>>2]=c[r+12>>2];c[s+16>>2]=c[r+16>>2];c[s+20>>2]=c[r+20>>2];c[s+24>>2]=c[r+24>>2];v=1;i=e;return v|0}r=m;m=a;c[r>>2]=c[m>>2];c[r+4>>2]=c[m+4>>2];c[r+8>>2]=c[m+8>>2];c[r+12>>2]=c[m+12>>2];c[r+16>>2]=c[m+16>>2];c[r+20>>2]=c[m+20>>2];c[r+24>>2]=c[m+24>>2];s=t;c[m>>2]=c[s>>2];c[m+4>>2]=c[s+4>>2];c[m+8>>2]=c[s+8>>2];c[m+12>>2]=c[s+12>>2];c[m+16>>2]=c[s+16>>2];c[m+20>>2]=c[s+20>>2];c[m+24>>2]=c[s+24>>2];c[s>>2]=c[r>>2];c[s+4>>2]=c[r+4>>2];c[s+8>>2]=c[r+8>>2];c[s+12>>2]=c[r+12>>2];c[s+16>>2]=c[r+16>>2];c[s+20>>2]=c[r+20>>2];c[s+24>>2]=c[r+24>>2];if(!(tb[c[d>>2]&63](w,t)|0)){v=1;i=e;return v|0}t=o;c[t>>2]=c[s>>2];c[t+4>>2]=c[s+4>>2];c[t+8>>2]=c[s+8>>2];c[t+12>>2]=c[s+12>>2];c[t+16>>2]=c[s+16>>2];c[t+20>>2]=c[s+20>>2];c[t+24>>2]=c[s+24>>2];o=w;c[s>>2]=c[o>>2];c[s+4>>2]=c[o+4>>2];c[s+8>>2]=c[o+8>>2];c[s+12>>2]=c[o+12>>2];c[s+16>>2]=c[o+16>>2];c[s+20>>2]=c[o+20>>2];c[s+24>>2]=c[o+24>>2];c[o>>2]=c[t>>2];c[o+4>>2]=c[t+4>>2];c[o+8>>2]=c[t+8>>2];c[o+12>>2]=c[t+12>>2];c[o+16>>2]=c[t+16>>2];c[o+20>>2]=c[t+20>>2];c[o+24>>2]=c[t+24>>2];v=1;i=e;return v|0};default:{t=a+56|0;o=a+28|0;s=tb[c[d>>2]&63](o,a)|0;w=tb[c[d>>2]&63](t,o)|0;do{if(s){if(w){r=f;m=a;c[r>>2]=c[m>>2];c[r+4>>2]=c[m+4>>2];c[r+8>>2]=c[m+8>>2];c[r+12>>2]=c[m+12>>2];c[r+16>>2]=c[m+16>>2];c[r+20>>2]=c[m+20>>2];c[r+24>>2]=c[m+24>>2];l=t;c[m>>2]=c[l>>2];c[m+4>>2]=c[l+4>>2];c[m+8>>2]=c[l+8>>2];c[m+12>>2]=c[l+12>>2];c[m+16>>2]=c[l+16>>2];c[m+20>>2]=c[l+20>>2];c[m+24>>2]=c[l+24>>2];c[l>>2]=c[r>>2];c[l+4>>2]=c[r+4>>2];c[l+8>>2]=c[r+8>>2];c[l+12>>2]=c[r+12>>2];c[l+16>>2]=c[r+16>>2];c[l+20>>2]=c[r+20>>2];c[l+24>>2]=c[r+24>>2];break}r=g;l=a;c[r>>2]=c[l>>2];c[r+4>>2]=c[l+4>>2];c[r+8>>2]=c[l+8>>2];c[r+12>>2]=c[l+12>>2];c[r+16>>2]=c[l+16>>2];c[r+20>>2]=c[l+20>>2];c[r+24>>2]=c[l+24>>2];m=o;c[l>>2]=c[m>>2];c[l+4>>2]=c[m+4>>2];c[l+8>>2]=c[m+8>>2];c[l+12>>2]=c[m+12>>2];c[l+16>>2]=c[m+16>>2];c[l+20>>2]=c[m+20>>2];c[l+24>>2]=c[m+24>>2];c[m>>2]=c[r>>2];c[m+4>>2]=c[r+4>>2];c[m+8>>2]=c[r+8>>2];c[m+12>>2]=c[r+12>>2];c[m+16>>2]=c[r+16>>2];c[m+20>>2]=c[r+20>>2];c[m+24>>2]=c[r+24>>2];if(!(tb[c[d>>2]&63](t,o)|0)){break}r=j;c[r>>2]=c[m>>2];c[r+4>>2]=c[m+4>>2];c[r+8>>2]=c[m+8>>2];c[r+12>>2]=c[m+12>>2];c[r+16>>2]=c[m+16>>2];c[r+20>>2]=c[m+20>>2];c[r+24>>2]=c[m+24>>2];l=t;c[m>>2]=c[l>>2];c[m+4>>2]=c[l+4>>2];c[m+8>>2]=c[l+8>>2];c[m+12>>2]=c[l+12>>2];c[m+16>>2]=c[l+16>>2];c[m+20>>2]=c[l+20>>2];c[m+24>>2]=c[l+24>>2];c[l>>2]=c[r>>2];c[l+4>>2]=c[r+4>>2];c[l+8>>2]=c[r+8>>2];c[l+12>>2]=c[r+12>>2];c[l+16>>2]=c[r+16>>2];c[l+20>>2]=c[r+20>>2];c[l+24>>2]=c[r+24>>2];}else{if(!w){break}r=k;l=o;c[r>>2]=c[l>>2];c[r+4>>2]=c[l+4>>2];c[r+8>>2]=c[l+8>>2];c[r+12>>2]=c[l+12>>2];c[r+16>>2]=c[l+16>>2];c[r+20>>2]=c[l+20>>2];c[r+24>>2]=c[l+24>>2];m=t;c[l>>2]=c[m>>2];c[l+4>>2]=c[m+4>>2];c[l+8>>2]=c[m+8>>2];c[l+12>>2]=c[m+12>>2];c[l+16>>2]=c[m+16>>2];c[l+20>>2]=c[m+20>>2];c[l+24>>2]=c[m+24>>2];c[m>>2]=c[r>>2];c[m+4>>2]=c[r+4>>2];c[m+8>>2]=c[r+8>>2];c[m+12>>2]=c[r+12>>2];c[m+16>>2]=c[r+16>>2];c[m+20>>2]=c[r+20>>2];c[m+24>>2]=c[r+24>>2];if(!(tb[c[d>>2]&63](o,a)|0)){break}r=h;m=a;c[r>>2]=c[m>>2];c[r+4>>2]=c[m+4>>2];c[r+8>>2]=c[m+8>>2];c[r+12>>2]=c[m+12>>2];c[r+16>>2]=c[m+16>>2];c[r+20>>2]=c[m+20>>2];c[r+24>>2]=c[m+24>>2];c[m>>2]=c[l>>2];c[m+4>>2]=c[l+4>>2];c[m+8>>2]=c[l+8>>2];c[m+12>>2]=c[l+12>>2];c[m+16>>2]=c[l+16>>2];c[m+20>>2]=c[l+20>>2];c[m+24>>2]=c[l+24>>2];c[l>>2]=c[r>>2];c[l+4>>2]=c[r+4>>2];c[l+8>>2]=c[r+8>>2];c[l+12>>2]=c[r+12>>2];c[l+16>>2]=c[r+16>>2];c[l+20>>2]=c[r+20>>2];c[l+24>>2]=c[r+24>>2];}}while(0);h=a+84|0;if((h|0)==(b|0)){v=1;i=e;return v|0}o=q;k=t;t=0;w=h;while(1){if(tb[c[d>>2]&63](w,k)|0){h=w;c[o>>2]=c[h>>2];c[o+4>>2]=c[h+4>>2];c[o+8>>2]=c[h+8>>2];c[o+12>>2]=c[h+12>>2];c[o+16>>2]=c[h+16>>2];c[o+20>>2]=c[h+20>>2];c[o+24>>2]=c[h+24>>2];h=k;j=w;while(1){g=j;z=h;c[g>>2]=c[z>>2];c[g+4>>2]=c[z+4>>2];c[g+8>>2]=c[z+8>>2];c[g+12>>2]=c[z+12>>2];c[g+16>>2]=c[z+16>>2];c[g+20>>2]=c[z+20>>2];c[g+24>>2]=c[z+24>>2];if((h|0)==(a|0)){break}g=h-28|0;if(tb[c[d>>2]&63](q,g)|0){j=h;h=g}else{break}}c[z>>2]=c[o>>2];c[z+4>>2]=c[o+4>>2];c[z+8>>2]=c[o+8>>2];c[z+12>>2]=c[o+12>>2];c[z+16>>2]=c[o+16>>2];c[z+20>>2]=c[o+20>>2];c[z+24>>2]=c[o+24>>2];h=t+1|0;if((h|0)==8){break}else{A=h}}else{A=t}h=w+28|0;if((h|0)==(b|0)){v=1;B=35;break}else{k=w;t=A;w=h}}if((B|0)==35){i=e;return v|0}v=(w+28|0)==(b|0);i=e;return v|0}}return 0}function Pl(a,b,d){a=a|0;b=b|0;d=d|0;var e=0,f=0,g=0,h=0,j=0,k=0,l=0,m=0,n=0,o=0,p=0,q=0,r=0,s=0,t=0,u=0,v=0,w=0,x=0,y=0,z=0,A=0,B=0,C=0,D=0,E=0,F=0,G=0,H=0,I=0,J=0,K=0,L=0,M=0,N=0,O=0,P=0,Q=0,R=0,S=0;e=i;i=i+8|0;f=e|0;g=a;a=b;a:while(1){b=a;h=a-8|0;j=h;k=g;b:while(1){l=k;m=b-l|0;n=m>>3;switch(n|0){case 3:{o=6;break a;break};case 2:{o=4;break a;break};case 4:{o=14;break a;break};case 5:{o=26;break a;break};case 0:case 1:{o=83;break a;break};default:{}}if((m|0)<248){o=28;break a}p=(n|0)/2|0;q=k+(p<<3)|0;do{if((m|0)>7992){r=(n|0)/4|0;s=Ql(k,k+(r<<3)|0,q,k+(r+p<<3)|0,h,d)|0}else{r=tb[c[d>>2]&63](q,k)|0;t=tb[c[d>>2]&63](h,q)|0;if(!r){if(!t){s=0;break}r=q;u=c[r>>2]|0;v=c[r+4>>2]|0;w=c[j+4>>2]|0;c[r>>2]=c[j>>2];c[r+4>>2]=w;c[j>>2]=u;c[j+4>>2]=v;if(!(tb[c[d>>2]&63](q,k)|0)){s=1;break}v=k;u=c[v>>2]|0;w=c[v+4>>2]|0;x=c[r+4>>2]|0;c[v>>2]=c[r>>2];c[v+4>>2]=x;c[r>>2]=u;c[r+4>>2]=w;s=2;break}w=k;r=c[w>>2]|0;u=c[w+4>>2]|0;if(t){t=c[j+4>>2]|0;c[w>>2]=c[j>>2];c[w+4>>2]=t;c[j>>2]=r;c[j+4>>2]=u;s=1;break}t=q;x=c[t+4>>2]|0;c[w>>2]=c[t>>2];c[w+4>>2]=x;c[t>>2]=r;c[t+4>>2]=u;if(!(tb[c[d>>2]&63](h,q)|0)){s=1;break}u=c[t>>2]|0;r=c[t+4>>2]|0;x=c[j+4>>2]|0;c[t>>2]=c[j>>2];c[t+4>>2]=x;c[j>>2]=u;c[j+4>>2]=r;s=2}}while(0);do{if(tb[c[d>>2]&63](k,q)|0){y=h;z=s}else{p=h;while(1){A=p-8|0;if((k|0)==(A|0)){break}if(tb[c[d>>2]&63](A,q)|0){o=66;break}else{p=A}}if((o|0)==66){o=0;p=k;n=c[p>>2]|0;m=c[p+4>>2]|0;r=A;u=c[r+4>>2]|0;c[p>>2]=c[r>>2];c[p+4>>2]=u;c[r>>2]=n;c[r+4>>2]=m;y=A;z=s+1|0;break}m=k+8|0;if(tb[c[d>>2]&63](k,h)|0){B=m}else{r=m;while(1){if((r|0)==(h|0)){o=83;break a}C=r+8|0;if(tb[c[d>>2]&63](k,r)|0){break}else{r=C}}m=r;n=c[m>>2]|0;u=c[m+4>>2]|0;p=c[j+4>>2]|0;c[m>>2]=c[j>>2];c[m+4>>2]=p;c[j>>2]=n;c[j+4>>2]=u;B=C}if((B|0)==(h|0)){o=83;break a}else{D=h;E=B}while(1){u=E;while(1){F=u+8|0;if(tb[c[d>>2]&63](k,u)|0){G=D;break}else{u=F}}do{G=G-8|0;}while(tb[c[d>>2]&63](k,G)|0);if(!(u>>>0<G>>>0)){k=u;continue b}n=u;p=c[n>>2]|0;m=c[n+4>>2]|0;x=G;t=c[x+4>>2]|0;c[n>>2]=c[x>>2];c[n+4>>2]=t;c[x>>2]=p;c[x+4>>2]=m;D=G;E=F}}}while(0);r=k+8|0;c:do{if(r>>>0<y>>>0){m=y;x=r;p=z;t=q;while(1){n=x;while(1){H=n+8|0;if(tb[c[d>>2]&63](n,t)|0){n=H}else{I=m;break}}do{I=I-8|0;}while(!(tb[c[d>>2]&63](I,t)|0));if(n>>>0>I>>>0){J=n;K=p;L=t;break c}u=n;w=c[u>>2]|0;v=c[u+4>>2]|0;M=I;N=c[M+4>>2]|0;c[u>>2]=c[M>>2];c[u+4>>2]=N;c[M>>2]=w;c[M+4>>2]=v;m=I;x=H;p=p+1|0;t=(t|0)==(n|0)?I:t}}else{J=r;K=z;L=q}}while(0);do{if((J|0)==(L|0)){O=K}else{if(!(tb[c[d>>2]&63](L,J)|0)){O=K;break}q=J;r=c[q>>2]|0;t=c[q+4>>2]|0;p=L;x=c[p+4>>2]|0;c[q>>2]=c[p>>2];c[q+4>>2]=x;c[p>>2]=r;c[p+4>>2]=t;O=K+1|0}}while(0);if((O|0)==0){P=Rl(k,J,d)|0;t=J+8|0;if(Rl(t,a,d)|0){o=78;break}if(P){k=t;continue}}t=J;if((t-l|0)>=(b-t|0)){o=82;break}Pl(k,J,d);k=J+8|0}if((o|0)==78){o=0;if(P){o=83;break}else{g=k;a=J;continue}}else if((o|0)==82){o=0;Pl(J+8|0,a,d);g=k;a=J;continue}}if((o|0)==4){if(!(tb[c[d>>2]&63](h,k)|0)){i=e;return}J=k;g=c[J>>2]|0;P=c[J+4>>2]|0;O=c[j+4>>2]|0;c[J>>2]=c[j>>2];c[J+4>>2]=O;c[j>>2]=g;c[j+4>>2]=P;i=e;return}else if((o|0)==6){P=k+8|0;g=tb[c[d>>2]&63](P,k)|0;O=tb[c[d>>2]&63](h,P)|0;if(!g){if(!O){i=e;return}g=P;J=c[g>>2]|0;K=c[g+4>>2]|0;L=c[j+4>>2]|0;c[g>>2]=c[j>>2];c[g+4>>2]=L;c[j>>2]=J;c[j+4>>2]=K;if(!(tb[c[d>>2]&63](P,k)|0)){i=e;return}K=k;J=c[K>>2]|0;L=c[K+4>>2]|0;z=c[g+4>>2]|0;c[K>>2]=c[g>>2];c[K+4>>2]=z;c[g>>2]=J;c[g+4>>2]=L;i=e;return}L=k;g=c[L>>2]|0;J=c[L+4>>2]|0;if(O){O=c[j+4>>2]|0;c[L>>2]=c[j>>2];c[L+4>>2]=O;c[j>>2]=g;c[j+4>>2]=J;i=e;return}O=P;z=c[O+4>>2]|0;c[L>>2]=c[O>>2];c[L+4>>2]=z;c[O>>2]=g;c[O+4>>2]=J;if(!(tb[c[d>>2]&63](h,P)|0)){i=e;return}P=c[O>>2]|0;J=c[O+4>>2]|0;g=c[j+4>>2]|0;c[O>>2]=c[j>>2];c[O+4>>2]=g;c[j>>2]=P;c[j+4>>2]=J;i=e;return}else if((o|0)==14){J=k+8|0;P=k+16|0;g=tb[c[d>>2]&63](J,k)|0;O=tb[c[d>>2]&63](P,J)|0;do{if(g){z=k;L=c[z>>2]|0;K=c[z+4>>2]|0;if(O){I=P;H=c[I+4>>2]|0;c[z>>2]=c[I>>2];c[z+4>>2]=H;c[I>>2]=L;c[I+4>>2]=K;break}I=J;H=c[I+4>>2]|0;c[z>>2]=c[I>>2];c[z+4>>2]=H;c[I>>2]=L;c[I+4>>2]=K;if(!(tb[c[d>>2]&63](P,J)|0)){break}K=c[I>>2]|0;L=c[I+4>>2]|0;H=P;z=c[H+4>>2]|0;c[I>>2]=c[H>>2];c[I+4>>2]=z;c[H>>2]=K;c[H+4>>2]=L}else{if(!O){break}L=J;H=c[L>>2]|0;K=c[L+4>>2]|0;z=P;I=c[z+4>>2]|0;c[L>>2]=c[z>>2];c[L+4>>2]=I;c[z>>2]=H;c[z+4>>2]=K;if(!(tb[c[d>>2]&63](J,k)|0)){break}K=k;z=c[K>>2]|0;H=c[K+4>>2]|0;I=c[L+4>>2]|0;c[K>>2]=c[L>>2];c[K+4>>2]=I;c[L>>2]=z;c[L+4>>2]=H}}while(0);if(!(tb[c[d>>2]&63](h,P)|0)){i=e;return}O=P;g=c[O>>2]|0;H=c[O+4>>2]|0;L=c[j+4>>2]|0;c[O>>2]=c[j>>2];c[O+4>>2]=L;c[j>>2]=g;c[j+4>>2]=H;if(!(tb[c[d>>2]&63](P,J)|0)){i=e;return}P=J;H=c[P>>2]|0;j=c[P+4>>2]|0;g=c[O+4>>2]|0;c[P>>2]=c[O>>2];c[P+4>>2]=g;c[O>>2]=H;c[O+4>>2]=j;if(!(tb[c[d>>2]&63](J,k)|0)){i=e;return}J=k;j=c[J>>2]|0;O=c[J+4>>2]|0;H=c[P+4>>2]|0;c[J>>2]=c[P>>2];c[J+4>>2]=H;c[P>>2]=j;c[P+4>>2]=O;i=e;return}else if((o|0)==26){Ql(k,k+8|0,k+16|0,k+24|0,h,d)|0;i=e;return}else if((o|0)==28){h=f;O=k+16|0;P=k+8|0;j=tb[c[d>>2]&63](P,k)|0;H=tb[c[d>>2]&63](O,P)|0;do{if(j){J=k;g=c[J>>2]|0;L=c[J+4>>2]|0;if(H){z=O;I=c[z+4>>2]|0;c[J>>2]=c[z>>2];c[J+4>>2]=I;c[z>>2]=g;c[z+4>>2]=L;break}z=P;I=c[z+4>>2]|0;c[J>>2]=c[z>>2];c[J+4>>2]=I;c[z>>2]=g;c[z+4>>2]=L;if(!(tb[c[d>>2]&63](O,P)|0)){break}L=c[z>>2]|0;g=c[z+4>>2]|0;I=O;J=c[I+4>>2]|0;c[z>>2]=c[I>>2];c[z+4>>2]=J;c[I>>2]=L;c[I+4>>2]=g}else{if(!H){break}g=P;I=c[g>>2]|0;L=c[g+4>>2]|0;J=O;z=c[J+4>>2]|0;c[g>>2]=c[J>>2];c[g+4>>2]=z;c[J>>2]=I;c[J+4>>2]=L;if(!(tb[c[d>>2]&63](P,k)|0)){break}L=k;J=c[L>>2]|0;I=c[L+4>>2]|0;z=c[g+4>>2]|0;c[L>>2]=c[g>>2];c[L+4>>2]=z;c[g>>2]=J;c[g+4>>2]=I}}while(0);P=k+24|0;if((P|0)==(a|0)){i=e;return}else{Q=O;R=P}while(1){if(tb[c[d>>2]&63](R,Q)|0){P=R;O=c[P+4>>2]|0;c[f>>2]=c[P>>2];c[f+4>>2]=O;O=Q;P=R;while(1){S=O;H=P;j=c[S+4>>2]|0;c[H>>2]=c[S>>2];c[H+4>>2]=j;if((O|0)==(k|0)){break}j=O-8|0;if(tb[c[d>>2]&63](h,j)|0){P=O;O=j}else{break}}O=c[f+4>>2]|0;c[S>>2]=c[f>>2];c[S+4>>2]=O}O=R+8|0;if((O|0)==(a|0)){break}else{Q=R;R=O}}i=e;return}else if((o|0)==83){i=e;return}}function Ql(a,b,d,e,f,g){a=a|0;b=b|0;d=d|0;e=e|0;f=f|0;g=g|0;var h=0,i=0,j=0,k=0,l=0,m=0,n=0,o=0,p=0,q=0;h=tb[c[g>>2]&63](b,a)|0;i=tb[c[g>>2]&63](d,b)|0;do{if(h){j=a;k=c[j>>2]|0;l=c[j+4>>2]|0;if(i){m=d;n=c[m+4>>2]|0;c[j>>2]=c[m>>2];c[j+4>>2]=n;c[m>>2]=k;c[m+4>>2]=l;o=1;break}m=b;n=c[m+4>>2]|0;c[j>>2]=c[m>>2];c[j+4>>2]=n;c[m>>2]=k;c[m+4>>2]=l;if(!(tb[c[g>>2]&63](d,b)|0)){o=1;break}l=c[m>>2]|0;k=c[m+4>>2]|0;n=d;j=c[n+4>>2]|0;c[m>>2]=c[n>>2];c[m+4>>2]=j;c[n>>2]=l;c[n+4>>2]=k;o=2}else{if(!i){o=0;break}k=b;n=c[k>>2]|0;l=c[k+4>>2]|0;j=d;m=c[j+4>>2]|0;c[k>>2]=c[j>>2];c[k+4>>2]=m;c[j>>2]=n;c[j+4>>2]=l;if(!(tb[c[g>>2]&63](b,a)|0)){o=1;break}l=a;j=c[l>>2]|0;n=c[l+4>>2]|0;m=c[k+4>>2]|0;c[l>>2]=c[k>>2];c[l+4>>2]=m;c[k>>2]=j;c[k+4>>2]=n;o=2}}while(0);do{if(tb[c[g>>2]&63](e,d)|0){i=d;h=c[i>>2]|0;n=c[i+4>>2]|0;k=e;j=c[k+4>>2]|0;c[i>>2]=c[k>>2];c[i+4>>2]=j;c[k>>2]=h;c[k+4>>2]=n;if(!(tb[c[g>>2]&63](d,b)|0)){p=o+1|0;break}n=b;k=c[n>>2]|0;h=c[n+4>>2]|0;j=c[i+4>>2]|0;c[n>>2]=c[i>>2];c[n+4>>2]=j;c[i>>2]=k;c[i+4>>2]=h;if(!(tb[c[g>>2]&63](b,a)|0)){p=o+2|0;break}h=a;i=c[h>>2]|0;k=c[h+4>>2]|0;j=c[n+4>>2]|0;c[h>>2]=c[n>>2];c[h+4>>2]=j;c[n>>2]=i;c[n+4>>2]=k;p=o+3|0}else{p=o}}while(0);if(!(tb[c[g>>2]&63](f,e)|0)){q=p;return q|0}o=e;k=c[o>>2]|0;n=c[o+4>>2]|0;i=f;f=c[i+4>>2]|0;c[o>>2]=c[i>>2];c[o+4>>2]=f;c[i>>2]=k;c[i+4>>2]=n;if(!(tb[c[g>>2]&63](e,d)|0)){q=p+1|0;return q|0}e=d;n=c[e>>2]|0;i=c[e+4>>2]|0;k=c[o+4>>2]|0;c[e>>2]=c[o>>2];c[e+4>>2]=k;c[o>>2]=n;c[o+4>>2]=i;if(!(tb[c[g>>2]&63](d,b)|0)){q=p+2|0;return q|0}d=b;i=c[d>>2]|0;o=c[d+4>>2]|0;n=c[e+4>>2]|0;c[d>>2]=c[e>>2];c[d+4>>2]=n;c[e>>2]=i;c[e+4>>2]=o;if(!(tb[c[g>>2]&63](b,a)|0)){q=p+3|0;return q|0}b=a;a=c[b>>2]|0;g=c[b+4>>2]|0;o=c[d+4>>2]|0;c[b>>2]=c[d>>2];c[b+4>>2]=o;c[d>>2]=a;c[d+4>>2]=g;q=p+4|0;return q|0}function Rl(a,b,d){a=a|0;b=b|0;d=d|0;var e=0,f=0,g=0,h=0,j=0,k=0,l=0,m=0,n=0,o=0,p=0,q=0,r=0,s=0,t=0,u=0,v=0,w=0,x=0,y=0;e=i;i=i+8|0;f=e|0;g=f;switch(b-a>>3|0){case 5:{Ql(a,a+8|0,a+16|0,a+24|0,b-8|0,d)|0;h=1;i=e;return h|0};case 0:case 1:{h=1;i=e;return h|0};case 3:{j=a+8|0;k=b-8|0;l=tb[c[d>>2]&63](j,a)|0;m=tb[c[d>>2]&63](k,j)|0;if(!l){if(!m){h=1;i=e;return h|0}l=j;n=c[l>>2]|0;o=c[l+4>>2]|0;p=k;q=c[p+4>>2]|0;c[l>>2]=c[p>>2];c[l+4>>2]=q;c[p>>2]=n;c[p+4>>2]=o;if(!(tb[c[d>>2]&63](j,a)|0)){h=1;i=e;return h|0}o=a;p=c[o>>2]|0;n=c[o+4>>2]|0;q=c[l+4>>2]|0;c[o>>2]=c[l>>2];c[o+4>>2]=q;c[l>>2]=p;c[l+4>>2]=n;h=1;i=e;return h|0}n=a;l=c[n>>2]|0;p=c[n+4>>2]|0;if(m){m=k;q=c[m+4>>2]|0;c[n>>2]=c[m>>2];c[n+4>>2]=q;c[m>>2]=l;c[m+4>>2]=p;h=1;i=e;return h|0}m=j;q=c[m+4>>2]|0;c[n>>2]=c[m>>2];c[n+4>>2]=q;c[m>>2]=l;c[m+4>>2]=p;if(!(tb[c[d>>2]&63](k,j)|0)){h=1;i=e;return h|0}j=c[m>>2]|0;p=c[m+4>>2]|0;l=k;k=c[l+4>>2]|0;c[m>>2]=c[l>>2];c[m+4>>2]=k;c[l>>2]=j;c[l+4>>2]=p;h=1;i=e;return h|0};case 4:{p=a+8|0;l=a+16|0;j=b-8|0;k=tb[c[d>>2]&63](p,a)|0;m=tb[c[d>>2]&63](l,p)|0;do{if(k){q=a;n=c[q>>2]|0;o=c[q+4>>2]|0;if(m){r=l;s=c[r+4>>2]|0;c[q>>2]=c[r>>2];c[q+4>>2]=s;c[r>>2]=n;c[r+4>>2]=o;break}r=p;s=c[r+4>>2]|0;c[q>>2]=c[r>>2];c[q+4>>2]=s;c[r>>2]=n;c[r+4>>2]=o;if(!(tb[c[d>>2]&63](l,p)|0)){break}o=c[r>>2]|0;n=c[r+4>>2]|0;s=l;q=c[s+4>>2]|0;c[r>>2]=c[s>>2];c[r+4>>2]=q;c[s>>2]=o;c[s+4>>2]=n}else{if(!m){break}n=p;s=c[n>>2]|0;o=c[n+4>>2]|0;q=l;r=c[q+4>>2]|0;c[n>>2]=c[q>>2];c[n+4>>2]=r;c[q>>2]=s;c[q+4>>2]=o;if(!(tb[c[d>>2]&63](p,a)|0)){break}o=a;q=c[o>>2]|0;s=c[o+4>>2]|0;r=c[n+4>>2]|0;c[o>>2]=c[n>>2];c[o+4>>2]=r;c[n>>2]=q;c[n+4>>2]=s}}while(0);if(!(tb[c[d>>2]&63](j,l)|0)){h=1;i=e;return h|0}m=l;k=c[m>>2]|0;s=c[m+4>>2]|0;n=j;j=c[n+4>>2]|0;c[m>>2]=c[n>>2];c[m+4>>2]=j;c[n>>2]=k;c[n+4>>2]=s;if(!(tb[c[d>>2]&63](l,p)|0)){h=1;i=e;return h|0}l=p;s=c[l>>2]|0;n=c[l+4>>2]|0;k=c[m+4>>2]|0;c[l>>2]=c[m>>2];c[l+4>>2]=k;c[m>>2]=s;c[m+4>>2]=n;if(!(tb[c[d>>2]&63](p,a)|0)){h=1;i=e;return h|0}p=a;n=c[p>>2]|0;m=c[p+4>>2]|0;s=c[l+4>>2]|0;c[p>>2]=c[l>>2];c[p+4>>2]=s;c[l>>2]=n;c[l+4>>2]=m;h=1;i=e;return h|0};case 2:{m=b-8|0;if(!(tb[c[d>>2]&63](m,a)|0)){h=1;i=e;return h|0}l=a;n=c[l>>2]|0;s=c[l+4>>2]|0;p=m;m=c[p+4>>2]|0;c[l>>2]=c[p>>2];c[l+4>>2]=m;c[p>>2]=n;c[p+4>>2]=s;h=1;i=e;return h|0};default:{s=a+16|0;p=a+8|0;n=tb[c[d>>2]&63](p,a)|0;m=tb[c[d>>2]&63](s,p)|0;do{if(n){l=a;k=c[l>>2]|0;j=c[l+4>>2]|0;if(m){q=s;r=c[q+4>>2]|0;c[l>>2]=c[q>>2];c[l+4>>2]=r;c[q>>2]=k;c[q+4>>2]=j;break}q=p;r=c[q+4>>2]|0;c[l>>2]=c[q>>2];c[l+4>>2]=r;c[q>>2]=k;c[q+4>>2]=j;if(!(tb[c[d>>2]&63](s,p)|0)){break}j=c[q>>2]|0;k=c[q+4>>2]|0;r=s;l=c[r+4>>2]|0;c[q>>2]=c[r>>2];c[q+4>>2]=l;c[r>>2]=j;c[r+4>>2]=k}else{if(!m){break}k=p;r=c[k>>2]|0;j=c[k+4>>2]|0;l=s;q=c[l+4>>2]|0;c[k>>2]=c[l>>2];c[k+4>>2]=q;c[l>>2]=r;c[l+4>>2]=j;if(!(tb[c[d>>2]&63](p,a)|0)){break}j=a;l=c[j>>2]|0;r=c[j+4>>2]|0;q=c[k+4>>2]|0;c[j>>2]=c[k>>2];c[j+4>>2]=q;c[k>>2]=l;c[k+4>>2]=r}}while(0);p=a+24|0;if((p|0)==(b|0)){h=1;i=e;return h|0}else{t=s;u=0;v=p}while(1){if(tb[c[d>>2]&63](v,t)|0){p=v;s=c[p+4>>2]|0;c[f>>2]=c[p>>2];c[f+4>>2]=s;s=t;p=v;while(1){w=s;m=p;n=c[w+4>>2]|0;c[m>>2]=c[w>>2];c[m+4>>2]=n;if((s|0)==(a|0)){break}n=s-8|0;if(tb[c[d>>2]&63](g,n)|0){p=s;s=n}else{break}}s=c[f+4>>2]|0;c[w>>2]=c[f>>2];c[w+4>>2]=s;s=u+1|0;if((s|0)==8){break}else{x=s}}else{x=u}s=v+8|0;if((s|0)==(b|0)){h=1;y=41;break}else{t=v;u=x;v=s}}if((y|0)==41){i=e;return h|0}h=(v+8|0)==(b|0);i=e;return h|0}}return 0}function Sl(a,b,d){a=a|0;b=b|0;d=d|0;var e=0,f=0,g=0,h=0,i=0,j=0,k=0,l=0,m=0,n=0,o=0,p=0,q=0,r=0,s=0,t=0,u=0,v=0,w=0,x=0,y=0,z=0,A=0,B=0,C=0,D=0,E=0,F=0,G=0,H=0,I=0,J=0,K=0,L=0,M=0,N=0,O=0,P=0,Q=0,R=0;e=a;a=b;a:while(1){b=a;f=a-8|0;g=a-8+4|0;h=f;i=e;b:while(1){j=i;k=b-j|0;l=k>>3;switch(l|0){case 0:case 1:{m=70;break a;break};case 3:{m=6;break a;break};case 5:{m=15;break a;break};case 2:{m=4;break a;break};case 4:{m=14;break a;break};default:{}}if((k|0)<248){m=21;break a}n=(l|0)/2|0;o=i+(n<<3)|0;do{if((k|0)>7992){p=(l|0)/4|0;q=i+(p<<3)|0;r=p+n|0;s=i+(r<<3)|0;t=Tl(i,q,o,s,0)|0;u=i+(r<<3)+4|0;if(!((c[g>>2]|0)>>>0<(c[u>>2]|0)>>>0)){v=t;w=i+(n<<3)+4|0;break}r=s;s=c[r>>2]|0;x=c[r+4>>2]|0;y=c[h+4>>2]|0;c[r>>2]=c[h>>2];c[r+4>>2]=y;c[h>>2]=s;c[h+4>>2]=x;x=i+(n<<3)+4|0;if(!((c[u>>2]|0)>>>0<(c[x>>2]|0)>>>0)){v=t+1|0;w=x;break}u=o;s=c[u>>2]|0;y=c[u+4>>2]|0;z=c[r+4>>2]|0;c[u>>2]=c[r>>2];c[u+4>>2]=z;c[r>>2]=s;c[r+4>>2]=y;y=i+(p<<3)+4|0;if(!((c[x>>2]|0)>>>0<(c[y>>2]|0)>>>0)){v=t+2|0;w=x;break}p=q;q=c[p>>2]|0;r=c[p+4>>2]|0;s=c[u+4>>2]|0;c[p>>2]=c[u>>2];c[p+4>>2]=s;c[u>>2]=q;c[u+4>>2]=r;if((c[y>>2]|0)>>>0<(c[i+4>>2]|0)>>>0){y=i;r=c[y>>2]|0;u=c[y+4>>2]|0;q=c[p+4>>2]|0;c[y>>2]=c[p>>2];c[y+4>>2]=q;c[p>>2]=r;c[p+4>>2]=u;v=t+4|0;w=x;break}else{v=t+3|0;w=x;break}}else{x=i+(n<<3)+4|0;t=c[x>>2]|0;u=i+4|0;p=(c[g>>2]|0)>>>0<t>>>0;if(!(t>>>0<(c[u>>2]|0)>>>0)){if(!p){v=0;w=x;break}t=o;r=c[t>>2]|0;q=c[t+4>>2]|0;y=c[h+4>>2]|0;c[t>>2]=c[h>>2];c[t+4>>2]=y;c[h>>2]=r;c[h+4>>2]=q;if(!((c[x>>2]|0)>>>0<(c[u>>2]|0)>>>0)){v=1;w=x;break}u=i;q=c[u>>2]|0;r=c[u+4>>2]|0;y=c[t+4>>2]|0;c[u>>2]=c[t>>2];c[u+4>>2]=y;c[t>>2]=q;c[t+4>>2]=r;v=2;w=x;break}r=i;t=c[r>>2]|0;q=c[r+4>>2]|0;if(p){p=c[h+4>>2]|0;c[r>>2]=c[h>>2];c[r+4>>2]=p;c[h>>2]=t;c[h+4>>2]=q;v=1;w=x;break}p=o;y=c[p+4>>2]|0;c[r>>2]=c[p>>2];c[r+4>>2]=y;c[p>>2]=t;c[p+4>>2]=q;if(!((c[g>>2]|0)>>>0<q>>>0)){v=1;w=x;break}y=c[h+4>>2]|0;c[p>>2]=c[h>>2];c[p+4>>2]=y;c[h>>2]=t;c[h+4>>2]=q;v=2;w=x}}while(0);n=i+4|0;l=c[n>>2]|0;k=c[w>>2]|0;do{if(l>>>0<k>>>0){A=f;B=v}else{x=f;while(1){C=x-8|0;if((i|0)==(C|0)){break}if((c[x-8+4>>2]|0)>>>0<k>>>0){m=53;break}else{x=C}}if((m|0)==53){m=0;x=i;q=c[x>>2]|0;t=c[x+4>>2]|0;y=C;p=c[y+4>>2]|0;c[x>>2]=c[y>>2];c[x+4>>2]=p;c[y>>2]=q;c[y+4>>2]=t;A=C;B=v+1|0;break}t=i+8|0;if(l>>>0<(c[g>>2]|0)>>>0){D=t}else{y=t;while(1){if((y|0)==(f|0)){m=70;break a}E=y+8|0;if(l>>>0<(c[y+4>>2]|0)>>>0){break}else{y=E}}t=y;q=c[t>>2]|0;p=c[t+4>>2]|0;x=c[h+4>>2]|0;c[t>>2]=c[h>>2];c[t+4>>2]=x;c[h>>2]=q;c[h+4>>2]=p;D=E}if((D|0)==(f|0)){m=70;break a}else{F=f;G=D}while(1){p=c[n>>2]|0;q=G;while(1){H=q+8|0;if(p>>>0<(c[q+4>>2]|0)>>>0){I=F;break}else{q=H}}while(1){J=I-8|0;if(p>>>0<(c[I-8+4>>2]|0)>>>0){I=J}else{break}}if(!(q>>>0<J>>>0)){i=q;continue b}p=q;x=c[p>>2]|0;t=c[p+4>>2]|0;r=J;u=c[r+4>>2]|0;c[p>>2]=c[r>>2];c[p+4>>2]=u;c[r>>2]=x;c[r+4>>2]=t;F=J;G=H}}}while(0);n=i+8|0;c:do{if(n>>>0<A>>>0){l=A;k=n;y=B;t=o;while(1){r=c[t+4>>2]|0;x=k;while(1){K=x+8|0;if((c[x+4>>2]|0)>>>0<r>>>0){x=K}else{L=l;break}}while(1){M=L-8|0;if((c[L-8+4>>2]|0)>>>0<r>>>0){break}else{L=M}}if(x>>>0>M>>>0){N=x;O=y;P=t;break c}r=x;q=c[r>>2]|0;u=c[r+4>>2]|0;p=M;s=c[p+4>>2]|0;c[r>>2]=c[p>>2];c[r+4>>2]=s;c[p>>2]=q;c[p+4>>2]=u;l=M;k=K;y=y+1|0;t=(t|0)==(x|0)?M:t}}else{N=n;O=B;P=o}}while(0);do{if((N|0)==(P|0)){Q=O}else{if(!((c[P+4>>2]|0)>>>0<(c[N+4>>2]|0)>>>0)){Q=O;break}o=N;n=c[o>>2]|0;t=c[o+4>>2]|0;y=P;k=c[y+4>>2]|0;c[o>>2]=c[y>>2];c[o+4>>2]=k;c[y>>2]=n;c[y+4>>2]=t;Q=O+1|0}}while(0);if((Q|0)==0){R=Vl(i,N,0)|0;t=N+8|0;if(Vl(t,a,0)|0){m=65;break}if(R){i=t;continue}}t=N;if((t-j|0)>=(b-t|0)){m=69;break}Sl(i,N,d);i=N+8|0}if((m|0)==65){m=0;if(R){m=70;break}else{e=i;a=N;continue}}else if((m|0)==69){m=0;Sl(N+8|0,a,d);e=i;a=N;continue}}if((m|0)==4){if(!((c[g>>2]|0)>>>0<(c[i+4>>2]|0)>>>0)){return}N=i;e=c[N>>2]|0;d=c[N+4>>2]|0;R=c[h+4>>2]|0;c[N>>2]=c[h>>2];c[N+4>>2]=R;c[h>>2]=e;c[h+4>>2]=d;return}else if((m|0)==6){d=i+8|0;e=i+12|0;R=c[e>>2]|0;N=i+4|0;Q=(c[g>>2]|0)>>>0<R>>>0;if(!(R>>>0<(c[N>>2]|0)>>>0)){if(!Q){return}R=d;O=c[R>>2]|0;P=c[R+4>>2]|0;B=c[h+4>>2]|0;c[R>>2]=c[h>>2];c[R+4>>2]=B;c[h>>2]=O;c[h+4>>2]=P;if(!((c[e>>2]|0)>>>0<(c[N>>2]|0)>>>0)){return}N=i;e=c[N>>2]|0;P=c[N+4>>2]|0;O=c[R+4>>2]|0;c[N>>2]=c[R>>2];c[N+4>>2]=O;c[R>>2]=e;c[R+4>>2]=P;return}P=i;R=c[P>>2]|0;e=c[P+4>>2]|0;if(Q){Q=c[h+4>>2]|0;c[P>>2]=c[h>>2];c[P+4>>2]=Q;c[h>>2]=R;c[h+4>>2]=e;return}Q=d;d=c[Q+4>>2]|0;c[P>>2]=c[Q>>2];c[P+4>>2]=d;c[Q>>2]=R;c[Q+4>>2]=e;if(!((c[g>>2]|0)>>>0<e>>>0)){return}d=c[h+4>>2]|0;c[Q>>2]=c[h>>2];c[Q+4>>2]=d;c[h>>2]=R;c[h+4>>2]=e;return}else if((m|0)==14){Tl(i,i+8|0,i+16|0,f,0)|0;return}else if((m|0)==15){f=i+8|0;e=i+16|0;R=i+24|0;Tl(i,f,e,R,0)|0;d=i+28|0;if(!((c[g>>2]|0)>>>0<(c[d>>2]|0)>>>0)){return}g=R;R=c[g>>2]|0;Q=c[g+4>>2]|0;P=c[h+4>>2]|0;c[g>>2]=c[h>>2];c[g+4>>2]=P;c[h>>2]=R;c[h+4>>2]=Q;if(!((c[d>>2]|0)>>>0<(c[i+20>>2]|0)>>>0)){return}d=e;e=c[d>>2]|0;Q=c[d+4>>2]|0;h=c[g>>2]|0;R=c[g+4>>2]|0;c[d>>2]=h;c[d+4>>2]=R;c[g>>2]=e;c[g+4>>2]=Q;Q=R;if(!(Q>>>0<(c[i+12>>2]|0)>>>0)){return}g=f;f=c[g>>2]|0;e=c[g+4>>2]|0;c[g>>2]=h;c[g+4>>2]=R;c[d>>2]=f;c[d+4>>2]=e;if(!(Q>>>0<(c[i+4>>2]|0)>>>0)){return}Q=i;e=c[Q>>2]|0;d=c[Q+4>>2]|0;c[Q>>2]=h;c[Q+4>>2]=R;c[g>>2]=e;c[g+4>>2]=d;return}else if((m|0)==21){Ul(i,a,0);return}else if((m|0)==70){return}}function Tl(a,b,d,e,f){a=a|0;b=b|0;d=d|0;e=e|0;f=f|0;var g=0,h=0,i=0,j=0,k=0,l=0,m=0,n=0,o=0,p=0,q=0,r=0,s=0,t=0;f=b+4|0;g=c[f>>2]|0;h=a+4|0;i=d+4|0;j=c[i>>2]|0;k=j>>>0<g>>>0;do{if(g>>>0<(c[h>>2]|0)>>>0){l=a;m=c[l>>2]|0;n=c[l+4>>2]|0;if(k){o=d;p=c[o+4>>2]|0;c[l>>2]=c[o>>2];c[l+4>>2]=p;c[o>>2]=m;c[o+4>>2]=n;q=1;r=n;break}o=b;p=c[o+4>>2]|0;c[l>>2]=c[o>>2];c[l+4>>2]=p;c[o>>2]=m;c[o+4>>2]=n;p=c[i>>2]|0;l=n;if(!(p>>>0<l>>>0)){q=1;r=p;break}p=d;s=c[p+4>>2]|0;c[o>>2]=c[p>>2];c[o+4>>2]=s;c[p>>2]=m;c[p+4>>2]=n;q=2;r=l}else{if(!k){q=0;r=j;break}l=b;n=c[l>>2]|0;p=c[l+4>>2]|0;m=d;s=c[m+4>>2]|0;c[l>>2]=c[m>>2];c[l+4>>2]=s;c[m>>2]=n;c[m+4>>2]=p;if(!((c[f>>2]|0)>>>0<(c[h>>2]|0)>>>0)){q=1;r=p;break}p=a;m=c[p>>2]|0;n=c[p+4>>2]|0;s=c[l+4>>2]|0;c[p>>2]=c[l>>2];c[p+4>>2]=s;c[l>>2]=m;c[l+4>>2]=n;q=2;r=c[i>>2]|0}}while(0);if(!((c[e+4>>2]|0)>>>0<r>>>0)){t=q;return t|0}r=d;d=c[r>>2]|0;j=c[r+4>>2]|0;k=e;e=c[k+4>>2]|0;c[r>>2]=c[k>>2];c[r+4>>2]=e;c[k>>2]=d;c[k+4>>2]=j;if(!((c[i>>2]|0)>>>0<(c[f>>2]|0)>>>0)){t=q+1|0;return t|0}i=b;b=c[i>>2]|0;j=c[i+4>>2]|0;k=c[r+4>>2]|0;c[i>>2]=c[r>>2];c[i+4>>2]=k;c[r>>2]=b;c[r+4>>2]=j;if(!((c[f>>2]|0)>>>0<(c[h>>2]|0)>>>0)){t=q+2|0;return t|0}h=a;a=c[h>>2]|0;f=c[h+4>>2]|0;j=c[i+4>>2]|0;c[h>>2]=c[i>>2];c[h+4>>2]=j;c[i>>2]=a;c[i+4>>2]=f;t=q+3|0;return t|0}function Ul(a,b,d){a=a|0;b=b|0;d=d|0;var e=0,f=0,g=0,h=0,i=0,j=0,k=0,l=0,m=0,n=0,o=0,p=0,q=0,r=0;d=a+16|0;e=a+8|0;f=c[a+12>>2]|0;g=c[a+4>>2]|0;h=c[a+20>>2]|0;i=h>>>0<f>>>0;do{if(f>>>0<g>>>0){j=a;k=c[j>>2]|0;l=c[j+4>>2]|0;if(i){m=d;n=c[m+4>>2]|0;c[j>>2]=c[m>>2];c[j+4>>2]=n;c[m>>2]=k;c[m+4>>2]=l;break}m=e;n=c[m+4>>2]|0;c[j>>2]=c[m>>2];c[j+4>>2]=n;c[m>>2]=k;c[m+4>>2]=l;if(!(h>>>0<l>>>0)){break}n=d;j=c[n+4>>2]|0;c[m>>2]=c[n>>2];c[m+4>>2]=j;c[n>>2]=k;c[n+4>>2]=l}else{if(!i){break}l=e;n=c[l>>2]|0;k=c[l+4>>2]|0;j=d;m=c[j>>2]|0;o=c[j+4>>2]|0;c[l>>2]=m;c[l+4>>2]=o;c[j>>2]=n;c[j+4>>2]=k;if(!(o>>>0<g>>>0)){break}k=a;j=c[k>>2]|0;n=c[k+4>>2]|0;c[k>>2]=m;c[k+4>>2]=o;c[l>>2]=j;c[l+4>>2]=n}}while(0);g=a+24|0;if((g|0)==(b|0)){return}else{p=d;q=g}while(1){if((c[q+4>>2]|0)>>>0<(c[p+4>>2]|0)>>>0){g=q;d=c[g>>2]|0;e=c[g+4>>2]|0;g=e;i=g;h=p;f=q;while(1){r=h;n=f;l=c[r+4>>2]|0;c[n>>2]=c[r>>2];c[n+4>>2]=l;if((h|0)==(a|0)){break}if(i>>>0<(c[h-8+4>>2]|0)>>>0){f=h;h=h-8|0}else{break}}c[r>>2]=d|0;c[r+4>>2]=g|e&0}h=q+8|0;if((h|0)==(b|0)){break}else{p=q;q=h}}return}function Vl(a,b,d){a=a|0;b=b|0;d=d|0;var e=0,f=0,g=0,h=0,i=0,j=0,k=0,l=0,m=0,n=0,o=0,p=0,q=0,r=0,s=0,t=0,u=0,v=0;switch(b-a>>3|0){case 0:case 1:{e=1;return e|0};case 3:{d=a+8|0;f=b-8|0;g=a+12|0;h=c[g>>2]|0;i=a+4|0;j=b-8+4|0;k=(c[j>>2]|0)>>>0<h>>>0;if(!(h>>>0<(c[i>>2]|0)>>>0)){if(!k){e=1;return e|0}h=d;l=c[h>>2]|0;m=c[h+4>>2]|0;n=f;o=c[n+4>>2]|0;c[h>>2]=c[n>>2];c[h+4>>2]=o;c[n>>2]=l;c[n+4>>2]=m;if(!((c[g>>2]|0)>>>0<(c[i>>2]|0)>>>0)){e=1;return e|0}i=a;g=c[i>>2]|0;m=c[i+4>>2]|0;n=c[h+4>>2]|0;c[i>>2]=c[h>>2];c[i+4>>2]=n;c[h>>2]=g;c[h+4>>2]=m;e=1;return e|0}m=a;h=c[m>>2]|0;g=c[m+4>>2]|0;if(k){k=f;n=c[k+4>>2]|0;c[m>>2]=c[k>>2];c[m+4>>2]=n;c[k>>2]=h;c[k+4>>2]=g;e=1;return e|0}k=d;d=c[k+4>>2]|0;c[m>>2]=c[k>>2];c[m+4>>2]=d;c[k>>2]=h;c[k+4>>2]=g;if(!((c[j>>2]|0)>>>0<g>>>0)){e=1;return e|0}j=f;f=c[j+4>>2]|0;c[k>>2]=c[j>>2];c[k+4>>2]=f;c[j>>2]=h;c[j+4>>2]=g;e=1;return e|0};case 4:{Tl(a,a+8|0,a+16|0,b-8|0,0)|0;e=1;return e|0};case 5:{g=a+8|0;j=a+16|0;h=a+24|0;Tl(a,g,j,h,0)|0;f=a+28|0;if(!((c[b-8+4>>2]|0)>>>0<(c[f>>2]|0)>>>0)){e=1;return e|0}k=h;h=c[k>>2]|0;d=c[k+4>>2]|0;m=b-8|0;n=c[m+4>>2]|0;c[k>>2]=c[m>>2];c[k+4>>2]=n;c[m>>2]=h;c[m+4>>2]=d;if(!((c[f>>2]|0)>>>0<(c[a+20>>2]|0)>>>0)){e=1;return e|0}f=j;j=c[f>>2]|0;d=c[f+4>>2]|0;m=c[k>>2]|0;h=c[k+4>>2]|0;c[f>>2]=m;c[f+4>>2]=h;c[k>>2]=j;c[k+4>>2]=d;d=h;if(!(d>>>0<(c[a+12>>2]|0)>>>0)){e=1;return e|0}k=g;g=c[k>>2]|0;j=c[k+4>>2]|0;c[k>>2]=m;c[k+4>>2]=h;c[f>>2]=g;c[f+4>>2]=j;if(!(d>>>0<(c[a+4>>2]|0)>>>0)){e=1;return e|0}d=a;j=c[d>>2]|0;f=c[d+4>>2]|0;c[d>>2]=m;c[d+4>>2]=h;c[k>>2]=j;c[k+4>>2]=f;e=1;return e|0};case 2:{if(!((c[b-8+4>>2]|0)>>>0<(c[a+4>>2]|0)>>>0)){e=1;return e|0}f=a;k=c[f>>2]|0;j=c[f+4>>2]|0;h=b-8|0;d=c[h+4>>2]|0;c[f>>2]=c[h>>2];c[f+4>>2]=d;c[h>>2]=k;c[h+4>>2]=j;e=1;return e|0};default:{j=a+16|0;h=a+8|0;k=c[a+12>>2]|0;d=c[a+4>>2]|0;f=c[a+20>>2]|0;m=f>>>0<k>>>0;do{if(k>>>0<d>>>0){g=a;n=c[g>>2]|0;i=c[g+4>>2]|0;if(m){l=j;o=c[l+4>>2]|0;c[g>>2]=c[l>>2];c[g+4>>2]=o;c[l>>2]=n;c[l+4>>2]=i;break}l=h;o=c[l+4>>2]|0;c[g>>2]=c[l>>2];c[g+4>>2]=o;c[l>>2]=n;c[l+4>>2]=i;if(!(f>>>0<i>>>0)){break}o=j;g=c[o+4>>2]|0;c[l>>2]=c[o>>2];c[l+4>>2]=g;c[o>>2]=n;c[o+4>>2]=i}else{if(!m){break}i=h;o=c[i>>2]|0;n=c[i+4>>2]|0;g=j;l=c[g>>2]|0;p=c[g+4>>2]|0;c[i>>2]=l;c[i+4>>2]=p;c[g>>2]=o;c[g+4>>2]=n;if(!(p>>>0<d>>>0)){break}n=a;g=c[n>>2]|0;o=c[n+4>>2]|0;c[n>>2]=l;c[n+4>>2]=p;c[i>>2]=g;c[i+4>>2]=o}}while(0);d=a+24|0;if((d|0)==(b|0)){e=1;return e|0}else{q=j;r=0;s=d}while(1){if((c[s+4>>2]|0)>>>0<(c[q+4>>2]|0)>>>0){d=s;j=c[d>>2]|0;h=c[d+4>>2]|0;d=h;m=d;f=q;k=s;while(1){t=f;o=k;i=c[t+4>>2]|0;c[o>>2]=c[t>>2];c[o+4>>2]=i;if((f|0)==(a|0)){break}if(m>>>0<(c[f-8+4>>2]|0)>>>0){k=f;f=f-8|0}else{break}}c[t>>2]=j|0;c[t+4>>2]=d|h&0;f=r+1|0;if((f|0)==8){break}else{u=f}}else{u=r}f=s+8|0;if((f|0)==(b|0)){e=1;v=34;break}else{q=s;r=u;s=f}}if((v|0)==34){return e|0}e=(s+8|0)==(b|0);return e|0}}return 0}function Wl(a,b,d,e,f,g){a=a|0;b=b|0;d=d|0;e=e|0;f=f|0;g=g|0;var h=0,j=0,k=0,l=0,m=0,n=0,o=0,p=0,q=0,r=0,s=0,t=0,u=0,v=0,w=0,x=0;h=i;i=i+128|0;j=h|0;k=h+64|0;if((e|0)==0|(e|0)==1){i=h;return}else if((e|0)==2){l=b-60|0;if(!(tb[c[d>>2]&63](l,a)|0)){i=h;return}m=k;k=a;On(m|0,k|0,60)|0;n=l;On(k|0,n|0,60)|0;On(n|0,m|0,60)|0;i=h;return}else{if((e|0)<129){m=j;if((a|0)==(b|0)){i=h;return}n=a+60|0;if((n|0)==(b|0)){i=h;return}else{o=n}do{On(m|0,o|0,60)|0;a:do{if((o|0)==(a|0)){p=a}else{n=o;while(1){k=n-60|0;if(!(tb[c[d>>2]&63](j,k)|0)){p=n;break a}On(n|0,k|0,60)|0;if((k|0)==(a|0)){p=a;break}else{n=k}}}}while(0);On(p|0,m|0,60)|0;o=o+60|0;}while((o|0)!=(b|0));i=h;return}o=(e|0)/2|0;m=a+(o*60|0)|0;if((e|0)>(g|0)){Wl(a,m,d,o,f,g);p=e-o|0;Wl(m,b,d,p,f,g);Yl(a,m,b,d,o,p,f,g);i=h;return}Xl(a,m,d,o,f);g=f+(o*60|0)|0;Xl(m,b,d,e-o|0,g);o=f+(e*60|0)|0;b:do{if((e+1|0)>>>0<3>>>0){q=g;r=a}else{b=g;m=a;p=f;while(1){if((b|0)==(o|0)){break}j=m;if(tb[c[d>>2]&63](b,p)|0){On(j|0,b|0,60)|0;s=p;t=b+60|0}else{On(j|0,p|0,60)|0;s=p+60|0;t=b}j=m+60|0;if((s|0)==(g|0)){q=t;r=j;break b}else{b=t;m=j;p=s}}if((p|0)==(g|0)){i=h;return}else{u=m;v=p}while(1){On(u|0,v|0,60)|0;b=v+60|0;if((b|0)==(g|0)){break}else{u=u+60|0;v=b}}i=h;return}}while(0);if((q|0)==(o|0)){i=h;return}else{w=q;x=r}while(1){On(x|0,w|0,60)|0;r=w+60|0;if((r|0)==(o|0)){break}else{w=r;x=x+60|0}}i=h;return}}function Xl(a,b,d,e,f){a=a|0;b=b|0;d=d|0;e=e|0;f=f|0;var g=0,h=0,i=0,j=0,k=0,l=0,m=0,n=0,o=0,p=0,q=0,r=0,s=0,t=0,u=0,v=0;if((e|0)==0){return}else if((e|0)==2){g=b-60|0;h=(f|0)==0;if(tb[c[d>>2]&63](g,a)|0){if(!h){On(f|0,g|0,60)|0}i=f+60|0;if((i|0)==0){return}On(i|0,a|0,60)|0;return}else{if(!h){On(f|0,a|0,60)|0}h=f+60|0;if((h|0)==0){return}On(h|0,g|0,60)|0;return}}else if((e|0)==1){if((f|0)==0){return}On(f|0,a|0,60)|0;return}else{if((e|0)<9){if((a|0)==(b|0)){return}if((f|0)==0){j=0}else{On(f|0,a|0,60)|0;j=f}g=a+60|0;if((g|0)==(b|0)){return}else{k=j;l=g}while(1){g=k+60|0;j=(g|0)==0;do{if(tb[c[d>>2]&63](l,k)|0){if(!j){On(g|0,k|0,60)|0}a:do{if((k|0)==(f|0)){m=f}else{h=k;while(1){i=h-60|0;if(!(tb[c[d>>2]&63](l,i)|0)){m=h;break a}On(h|0,i|0,60)|0;if((i|0)==(f|0)){m=f;break}else{h=i}}}}while(0);On(m|0,l|0,60)|0;n=g}else{if(j){n=0;break}On(g|0,l|0,60)|0;n=g}}while(0);g=l+60|0;if((g|0)==(b|0)){break}else{k=n;l=g}}return}l=(e|0)/2|0;n=a+(l*60|0)|0;Wl(a,n,d,l,f,l);k=e-l|0;Wl(n,b,d,k,f+(l*60|0)|0,k);b:do{if((e+1|0)>>>0<3>>>0){o=f;p=n}else{k=f;l=n;m=a;while(1){if((l|0)==(b|0)){break}g=(k|0)==0;if(tb[c[d>>2]&63](l,m)|0){if(!g){On(k|0,l|0,60)|0}q=m;r=l+60|0}else{if(!g){On(k|0,m|0,60)|0}q=m+60|0;r=l}g=k+60|0;if((q|0)==(n|0)){o=g;p=r;break b}else{k=g;l=r;m=q}}if((m|0)==(n|0)){return}else{s=k;t=m}while(1){if((s|0)!=0){On(s|0,t|0,60)|0}l=t+60|0;if((l|0)==(n|0)){break}else{s=s+60|0;t=l}}return}}while(0);if((p|0)==(b|0)){return}else{u=o;v=p}while(1){if((u|0)!=0){On(u|0,v|0,60)|0}p=v+60|0;if((p|0)==(b|0)){break}else{u=u+60|0;v=p}}return}}function Yl(a,b,d,e,f,g,h,j){a=a|0;b=b|0;d=d|0;e=e|0;f=f|0;g=g|0;h=h|0;j=j|0;var k=0,l=0,m=0,n=0,o=0,p=0,q=0,r=0,s=0,t=0,u=0,v=0,w=0,x=0,y=0,z=0,A=0,B=0,C=0,D=0,E=0,F=0,G=0,H=0,I=0,J=0,K=0,L=0,M=0,N=0,O=0,P=0,Q=0,R=0,S=0,T=0,U=0,V=0,W=0,X=0;k=i;i=i+320|0;l=k+256|0;if((g|0)==0){i=k;return}m=k|0;n=k+64|0;o=k+192|0;p=k+128|0;q=g;g=f;f=d;d=b;b=a;a:while(1){a=f;r=q;s=g;t=d;u=b;while(1){if((s|0)==0){v=45;break a}else{w=s;x=u}while(1){if(tb[c[e>>2]&63](t,x)|0){break}y=w-1|0;if((y|0)==0){v=45;break a}else{w=y;x=x+60|0}}if(!((w|0)>(j|0)&(r|0)>(j|0))){v=8;break a}if((w|0)<(r|0)){y=(r|0)/2|0;z=t+(y*60|0)|0;A=x;B=x;C=(t-A|0)/60|0;b:while(1){D=C;while(1){if((D|0)==0){break b}E=(D|0)/2|0;if(tb[c[e>>2]&63](z,B+(E*60|0)|0)|0){D=E}else{break}}B=B+((E+1|0)*60|0)|0;C=D-1-E|0}F=y;G=(B-A|0)/60|0;H=z;I=B}else{if((w|0)==1){v=17;break a}C=(w|0)/2|0;J=x+(C*60|0)|0;K=t;L=t;M=(a-K|0)/60|0;c:while(1){N=M;while(1){if((N|0)==0){break c}O=(N|0)/2|0;if(tb[c[e>>2]&63](L+(O*60|0)|0,J)|0){break}else{N=O}}L=L+((O+1|0)*60|0)|0;M=N-1-O|0}F=(L-K|0)/60|0;G=C;H=L;I=J}P=w-G|0;Q=r-F|0;d:do{if((I|0)==(t|0)){R=H}else{if((t|0)==(H|0)){R=I;break}if((I+60|0)==(t|0)){M=I;On(m|0,M|0,60)|0;B=H-t|0;Pn(M|0,t|0,B|0)|0;M=I+(((B|0)/60|0)*60|0)|0;On(M|0,m|0,60)|0;R=M;break}if((t+60|0)==(H|0)){M=H-60|0;On(n|0,M|0,60)|0;B=M-I|0;M=H+(((B|0)/-60|0)*60|0)|0;z=I;Pn(M|0,z|0,B|0)|0;On(z|0,n|0,60)|0;R=M;break}M=t;z=(M-I|0)/60|0;B=H;A=(B-M|0)/60|0;if((z|0)==(A|0)){M=I;y=t;while(1){D=M;On(p|0,D|0,60)|0;S=y;On(D|0,S|0,60)|0;On(S|0,p|0,60)|0;S=M+60|0;if((S|0)==(t|0)){R=t;break d}else{M=S;y=y+60|0}}}else{T=z;U=A}while(1){y=(T|0)%(U|0)|0;if((y|0)==0){break}else{T=U;U=y}}if((U|0)!=0){y=z-1|0;M=I+(U*60|0)|0;while(1){N=M-60|0;On(o|0,N|0,60)|0;S=M+(y*60|0)|0;D=N;while(1){V=S;On(D|0,V|0,60)|0;W=(B-S|0)/60|0;if((z|0)<(W|0)){X=S+(z*60|0)|0}else{X=I+((z-W|0)*60|0)|0}if((X|0)==(N|0)){break}else{D=S;S=X}}On(V|0,o|0,60)|0;if((N|0)==(I|0)){break}else{M=N}}}R=I+(A*60|0)|0}}while(0);if((F+G|0)>=(Q+P|0)){break}Yl(x,I,R,e,G,F,h,j);if((r|0)==(F|0)){v=45;break a}else{r=Q;s=P;t=H;u=R}}Yl(R,H,f,e,P,Q,h,j);if((F|0)==0){v=45;break}else{q=F;g=G;f=R;d=I;b=x}}if((v|0)==8){Zl(x,t,f,e,w,r,h);i=k;return}else if((v|0)==17){h=l;l=x;On(h|0,l|0,60)|0;x=t;On(l|0,x|0,60)|0;On(x|0,h|0,60)|0;i=k;return}else if((v|0)==45){i=k;return}}function Zl(a,b,d,e,f,g,h){a=a|0;b=b|0;d=d|0;e=e|0;f=f|0;g=g|0;h=h|0;var j=0,k=0,l=0,m=0,n=0,o=0,p=0,q=0,r=0,s=0,t=0,u=0,v=0,w=0,x=0,y=0,z=0,A=0,B=0,C=0,D=0,E=0,F=0,G=0;j=i;k=a;l=b;m=i;i=i+8|0;n=m;o=i;i=i+8|0;p=o;q=i;i=i+8|0;r=q;s=i;i=i+8|0;t=s;u=i;i=i+8|0;v=i;i=i+4|0;i=i+7&-8;w=i;i=i+8|0;if((f|0)>(g|0)){if((b|0)==(d|0)){x=h}else{g=(((d-60+(-l|0)|0)>>>0)/60|0)+1|0;f=h;y=b;while(1){if((f|0)!=0){On(f|0,y|0,60)|0}z=y+60|0;if((z|0)==(d|0)){break}else{f=f+60|0;y=z}}x=h+(g*60|0)|0}g=l;c[m>>2]=g;c[m+4>>2]=g;g=k;c[o>>2]=g;c[o+4>>2]=g;g=x;c[q>>2]=g;c[q+4>>2]=g;g=h;c[s>>2]=g;c[s+4>>2]=g;c[u>>2]=d;c[u+4>>2]=d;c[v>>2]=e;_l(w,n,p,r,t,u,v);i=j;return}a:do{if((a|0)==(b|0)){A=a;B=b}else{v=(((b-60+(-k|0)|0)>>>0)/60|0)+1|0;u=a;t=h;while(1){if((t|0)!=0){On(t|0,u|0,60)|0}r=u+60|0;if((r|0)==(b|0)){break}else{u=r;t=t+60|0}}t=h+(v*60|0)|0;if((t|0)==(h|0)){A=a;B=b;break}else{C=a;D=h;E=b}while(1){if((E|0)==(d|0)){break}u=C;if(tb[c[e>>2]&63](E,D)|0){On(u|0,E|0,60)|0;F=D;G=E+60|0}else{On(u|0,D|0,60)|0;F=D+60|0;G=E}u=C+60|0;if((F|0)==(t|0)){A=u;B=G;break a}else{C=u;D=F;E=G}}Pn(C|0,D|0,t-D|0)|0;i=j;return}}while(0);Pn(A|0,B|0,d-B|0)|0;i=j;return}function _l(a,b,d,e,f,g,h){a=a|0;b=b|0;d=d|0;e=e|0;f=f|0;g=g|0;h=h|0;var j=0,k=0,l=0,m=0,n=0,o=0,p=0,q=0,r=0,s=0,t=0,u=0,v=0,w=0,x=0,y=0,z=0,A=0,B=0,C=0,D=0,E=0,F=0,G=0,H=0,I=0;j=i;k=b;b=i;i=i+8|0;c[b>>2]=c[k>>2];c[b+4>>2]=c[k+4>>2];k=d;d=i;i=i+8|0;c[d>>2]=c[k>>2];c[d+4>>2]=c[k+4>>2];k=e;e=i;i=i+8|0;c[e>>2]=c[k>>2];c[e+4>>2]=c[k+4>>2];k=f;f=i;i=i+8|0;c[f>>2]=c[k>>2];c[f+4>>2]=c[k+4>>2];k=g;g=i;i=i+8|0;c[g>>2]=c[k>>2];c[g+4>>2]=c[k+4>>2];k=h;h=i;i=i+4|0;i=i+7&-8;c[h>>2]=c[k>>2];k=b;l=c[k+4>>2]|0;m=c[d+4>>2]|0;d=m;n=e;o=c[n+4>>2]|0;p=f;a:do{if((l|0)==(d|0)){q=o;r=c[p+4>>2]|0}else{f=e+4|0;s=e|0;t=b+4|0;u=b|0;v=c[h>>2]|0;w=g+4|0;x=g|0;y=c[p+4>>2]|0;z=l;A=o;while(1){if((A|0)==(y|0)){break}B=(c[f>>2]|0)-60|0;c[s>>2]=B;C=(c[t>>2]|0)-60|0;c[u>>2]=C;D=tb[c[v>>2]&63](B,C)|0;E=(c[w>>2]|0)-60|0;c[x>>2]=E;if(D){c[u>>2]=C;On(E|0,C|0,60)|0;c[t>>2]=C}else{c[s>>2]=B;On(E|0,B|0,60)|0;c[f>>2]=B}c[w>>2]=E;E=c[k+4>>2]|0;B=c[n+4>>2]|0;if((E|0)==(d|0)){q=B;r=y;break a}else{z=E;A=B}}A=g;y=c[A+4>>2]|0;if((z|0)==(d|0)){F=c[A>>2]|0;G=y}else{A=((z-60+(-m|0)|0)>>>0)/60|0;w=y;f=z;do{w=w-60|0;f=f-60|0;On(w|0,f|0,60)|0;}while((f|0)!=(d|0));f=y+(~A*60|0)|0;F=f;G=f}f=a;c[f>>2]=F;c[f+4>>2]=G;i=j;return}}while(0);G=g;g=c[G+4>>2]|0;F=q;q=r;if((F|0)==(q|0)){H=c[G>>2]|0;I=g}else{G=((F-60+(-r|0)|0)>>>0)/60|0;r=g;d=F;do{r=r-60|0;d=d-60|0;On(r|0,d|0,60)|0;}while((d|0)!=(q|0));q=g+(~G*60|0)|0;H=q;I=q}q=a;c[q>>2]=H;c[q+4>>2]=I;i=j;return}function $l(a,b,d,e,f,g){a=a|0;b=b|0;d=d|0;e=e|0;f=f|0;g=g|0;var h=0,j=0,k=0,l=0,m=0,n=0,o=0,p=0,q=0,r=0,s=0,t=0,u=0,v=0,w=0,x=0,y=0;h=i;i=i+48|0;j=h|0;k=h+24|0;if((e|0)==0|(e|0)==1){i=h;return}else if((e|0)==2){l=b-20|0;if(!(tb[c[d>>2]&63](l,a)|0)){i=h;return}m=k;k=a;c[m>>2]=c[k>>2];c[m+4>>2]=c[k+4>>2];c[m+8>>2]=c[k+8>>2];c[m+12>>2]=c[k+12>>2];c[m+16>>2]=c[k+16>>2];n=l;c[k>>2]=c[n>>2];c[k+4>>2]=c[n+4>>2];c[k+8>>2]=c[n+8>>2];c[k+12>>2]=c[n+12>>2];c[k+16>>2]=c[n+16>>2];c[n>>2]=c[m>>2];c[n+4>>2]=c[m+4>>2];c[n+8>>2]=c[m+8>>2];c[n+12>>2]=c[m+12>>2];c[n+16>>2]=c[m+16>>2];i=h;return}else{if((e|0)<129){m=j;if((a|0)==(b|0)){i=h;return}n=a+20|0;if((n|0)==(b|0)){i=h;return}else{o=n}do{n=o;c[m>>2]=c[n>>2];c[m+4>>2]=c[n+4>>2];c[m+8>>2]=c[n+8>>2];c[m+12>>2]=c[n+12>>2];c[m+16>>2]=c[n+16>>2];a:do{if((o|0)==(a|0)){p=a}else{n=o;while(1){k=n-20|0;if(!(tb[c[d>>2]&63](j,k)|0)){p=n;break a}l=n;q=k;c[l>>2]=c[q>>2];c[l+4>>2]=c[q+4>>2];c[l+8>>2]=c[q+8>>2];c[l+12>>2]=c[q+12>>2];c[l+16>>2]=c[q+16>>2];if((k|0)==(a|0)){p=a;break}else{n=k}}}}while(0);n=p;c[n>>2]=c[m>>2];c[n+4>>2]=c[m+4>>2];c[n+8>>2]=c[m+8>>2];c[n+12>>2]=c[m+12>>2];c[n+16>>2]=c[m+16>>2];o=o+20|0;}while((o|0)!=(b|0));i=h;return}o=(e|0)/2|0;m=a+(o*20|0)|0;if((e|0)>(g|0)){$l(a,m,d,o,f,g);p=e-o|0;$l(m,b,d,p,f,g);bm(a,m,b,d,o,p,f,g);i=h;return}am(a,m,d,o,f);g=f+(o*20|0)|0;am(m,b,d,e-o|0,g);o=f+(e*20|0)|0;b:do{if((e+1|0)>>>0<3>>>0){r=g;s=a}else{b=g;m=a;p=f;while(1){if((b|0)==(o|0)){break}j=m;if(tb[c[d>>2]&63](b,p)|0){n=b;c[j>>2]=c[n>>2];c[j+4>>2]=c[n+4>>2];c[j+8>>2]=c[n+8>>2];c[j+12>>2]=c[n+12>>2];c[j+16>>2]=c[n+16>>2];t=p;u=b+20|0}else{n=p;c[j>>2]=c[n>>2];c[j+4>>2]=c[n+4>>2];c[j+8>>2]=c[n+8>>2];c[j+12>>2]=c[n+12>>2];c[j+16>>2]=c[n+16>>2];t=p+20|0;u=b}n=m+20|0;if((t|0)==(g|0)){r=u;s=n;break b}else{b=u;m=n;p=t}}if((p|0)==(g|0)){i=h;return}else{v=m;w=p}while(1){b=v;n=w;c[b>>2]=c[n>>2];c[b+4>>2]=c[n+4>>2];c[b+8>>2]=c[n+8>>2];c[b+12>>2]=c[n+12>>2];c[b+16>>2]=c[n+16>>2];n=w+20|0;if((n|0)==(g|0)){break}else{v=v+20|0;w=n}}i=h;return}}while(0);if((r|0)==(o|0)){i=h;return}else{x=r;y=s}while(1){s=y;r=x;c[s>>2]=c[r>>2];c[s+4>>2]=c[r+4>>2];c[s+8>>2]=c[r+8>>2];c[s+12>>2]=c[r+12>>2];c[s+16>>2]=c[r+16>>2];r=x+20|0;if((r|0)==(o|0)){break}else{x=r;y=y+20|0}}i=h;return}}function am(a,b,d,e,f){a=a|0;b=b|0;d=d|0;e=e|0;f=f|0;var g=0,h=0,i=0,j=0,k=0,l=0,m=0,n=0,o=0,p=0,q=0,r=0,s=0,t=0,u=0,v=0,w=0,x=0;if((e|0)==2){g=b-20|0;h=(f|0)==0;if(tb[c[d>>2]&63](g,a)|0){if(!h){i=f;j=g;c[i>>2]=c[j>>2];c[i+4>>2]=c[j+4>>2];c[i+8>>2]=c[j+8>>2];c[i+12>>2]=c[j+12>>2];c[i+16>>2]=c[j+16>>2]}j=f+20|0;if((j|0)==0){return}i=j;j=a;c[i>>2]=c[j>>2];c[i+4>>2]=c[j+4>>2];c[i+8>>2]=c[j+8>>2];c[i+12>>2]=c[j+12>>2];c[i+16>>2]=c[j+16>>2];return}else{if(!h){h=f;j=a;c[h>>2]=c[j>>2];c[h+4>>2]=c[j+4>>2];c[h+8>>2]=c[j+8>>2];c[h+12>>2]=c[j+12>>2];c[h+16>>2]=c[j+16>>2]}j=f+20|0;if((j|0)==0){return}h=j;j=g;c[h>>2]=c[j>>2];c[h+4>>2]=c[j+4>>2];c[h+8>>2]=c[j+8>>2];c[h+12>>2]=c[j+12>>2];c[h+16>>2]=c[j+16>>2];return}}else if((e|0)==0){return}else if((e|0)==1){if((f|0)==0){return}j=f;h=a;c[j>>2]=c[h>>2];c[j+4>>2]=c[h+4>>2];c[j+8>>2]=c[h+8>>2];c[j+12>>2]=c[h+12>>2];c[j+16>>2]=c[h+16>>2];return}else{if((e|0)<9){if((a|0)==(b|0)){return}if((f|0)==0){k=0}else{h=f;j=a;c[h>>2]=c[j>>2];c[h+4>>2]=c[j+4>>2];c[h+8>>2]=c[j+8>>2];c[h+12>>2]=c[j+12>>2];c[h+16>>2]=c[j+16>>2];k=f}j=a+20|0;if((j|0)==(b|0)){return}else{l=k;m=j}while(1){j=l+20|0;k=(j|0)==0;do{if(tb[c[d>>2]&63](m,l)|0){if(!k){h=j;g=l;c[h>>2]=c[g>>2];c[h+4>>2]=c[g+4>>2];c[h+8>>2]=c[g+8>>2];c[h+12>>2]=c[g+12>>2];c[h+16>>2]=c[g+16>>2]}a:do{if((l|0)==(f|0)){n=f}else{g=l;while(1){h=g-20|0;if(!(tb[c[d>>2]&63](m,h)|0)){n=g;break a}i=g;o=h;c[i>>2]=c[o>>2];c[i+4>>2]=c[o+4>>2];c[i+8>>2]=c[o+8>>2];c[i+12>>2]=c[o+12>>2];c[i+16>>2]=c[o+16>>2];if((h|0)==(f|0)){n=f;break}else{g=h}}}}while(0);g=n;h=m;c[g>>2]=c[h>>2];c[g+4>>2]=c[h+4>>2];c[g+8>>2]=c[h+8>>2];c[g+12>>2]=c[h+12>>2];c[g+16>>2]=c[h+16>>2];p=j}else{if(k){p=0;break}h=j;g=m;c[h>>2]=c[g>>2];c[h+4>>2]=c[g+4>>2];c[h+8>>2]=c[g+8>>2];c[h+12>>2]=c[g+12>>2];c[h+16>>2]=c[g+16>>2];p=j}}while(0);j=m+20|0;if((j|0)==(b|0)){break}else{l=p;m=j}}return}m=(e|0)/2|0;p=a+(m*20|0)|0;$l(a,p,d,m,f,m);l=e-m|0;$l(p,b,d,l,f+(m*20|0)|0,l);b:do{if((e+1|0)>>>0<3>>>0){q=f;r=p}else{l=f;m=p;n=a;while(1){if((m|0)==(b|0)){break}j=(l|0)==0;if(tb[c[d>>2]&63](m,n)|0){if(!j){k=l;g=m;c[k>>2]=c[g>>2];c[k+4>>2]=c[g+4>>2];c[k+8>>2]=c[g+8>>2];c[k+12>>2]=c[g+12>>2];c[k+16>>2]=c[g+16>>2]}s=n;t=m+20|0}else{if(!j){j=l;g=n;c[j>>2]=c[g>>2];c[j+4>>2]=c[g+4>>2];c[j+8>>2]=c[g+8>>2];c[j+12>>2]=c[g+12>>2];c[j+16>>2]=c[g+16>>2]}s=n+20|0;t=m}g=l+20|0;if((s|0)==(p|0)){q=g;r=t;break b}else{l=g;m=t;n=s}}if((n|0)==(p|0)){return}else{u=l;v=n}while(1){if((u|0)!=0){m=u;g=v;c[m>>2]=c[g>>2];c[m+4>>2]=c[g+4>>2];c[m+8>>2]=c[g+8>>2];c[m+12>>2]=c[g+12>>2];c[m+16>>2]=c[g+16>>2]}g=v+20|0;if((g|0)==(p|0)){break}else{u=u+20|0;v=g}}return}}while(0);if((r|0)==(b|0)){return}else{w=q;x=r}while(1){if((w|0)!=0){r=w;q=x;c[r>>2]=c[q>>2];c[r+4>>2]=c[q+4>>2];c[r+8>>2]=c[q+8>>2];c[r+12>>2]=c[q+12>>2];c[r+16>>2]=c[q+16>>2]}q=x+20|0;if((q|0)==(b|0)){break}else{w=w+20|0;x=q}}return}}function bm(a,b,d,e,f,g,h,j){a=a|0;b=b|0;d=d|0;e=e|0;f=f|0;g=g|0;h=h|0;j=j|0;var k=0,l=0,m=0,n=0,o=0,p=0,q=0,r=0,s=0,t=0,u=0,v=0,w=0,x=0,y=0,z=0,A=0,B=0,C=0,D=0,E=0,F=0,G=0,H=0,I=0,J=0,K=0,L=0,M=0,N=0,O=0,P=0,Q=0,R=0,S=0,T=0,U=0,V=0,W=0,X=0;k=i;i=i+120|0;l=k+96|0;if((g|0)==0){i=k;return}m=k|0;n=k+24|0;o=k+72|0;p=k+48|0;q=g;g=f;f=d;d=b;b=a;a:while(1){a=f;r=q;s=g;t=d;u=b;while(1){if((s|0)==0){v=45;break a}else{w=s;x=u}while(1){if(tb[c[e>>2]&63](t,x)|0){break}y=w-1|0;if((y|0)==0){v=45;break a}else{w=y;x=x+20|0}}if(!((w|0)>(j|0)&(r|0)>(j|0))){v=8;break a}if((w|0)<(r|0)){y=(r|0)/2|0;z=t+(y*20|0)|0;A=x;B=x;C=(t-A|0)/20|0;b:while(1){D=C;while(1){if((D|0)==0){break b}E=(D|0)/2|0;if(tb[c[e>>2]&63](z,B+(E*20|0)|0)|0){D=E}else{break}}B=B+((E+1|0)*20|0)|0;C=D-1-E|0}F=y;G=(B-A|0)/20|0;H=z;I=B}else{if((w|0)==1){v=17;break a}C=(w|0)/2|0;J=x+(C*20|0)|0;K=t;L=t;M=(a-K|0)/20|0;c:while(1){N=M;while(1){if((N|0)==0){break c}O=(N|0)/2|0;if(tb[c[e>>2]&63](L+(O*20|0)|0,J)|0){break}else{N=O}}L=L+((O+1|0)*20|0)|0;M=N-1-O|0}F=(L-K|0)/20|0;G=C;H=L;I=J}P=w-G|0;Q=r-F|0;d:do{if((I|0)==(t|0)){R=H}else{if((t|0)==(H|0)){R=I;break}if((I+20|0)==(t|0)){M=I;c[m>>2]=c[M>>2];c[m+4>>2]=c[M+4>>2];c[m+8>>2]=c[M+8>>2];c[m+12>>2]=c[M+12>>2];c[m+16>>2]=c[M+16>>2];B=H-t|0;Pn(M|0,t|0,B|0)|0;M=I+(((B|0)/20|0)*20|0)|0;B=M;c[B>>2]=c[m>>2];c[B+4>>2]=c[m+4>>2];c[B+8>>2]=c[m+8>>2];c[B+12>>2]=c[m+12>>2];c[B+16>>2]=c[m+16>>2];R=M;break}if((t+20|0)==(H|0)){M=H-20|0;B=M;c[n>>2]=c[B>>2];c[n+4>>2]=c[B+4>>2];c[n+8>>2]=c[B+8>>2];c[n+12>>2]=c[B+12>>2];c[n+16>>2]=c[B+16>>2];B=M-I|0;M=H+(((B|0)/-20|0)*20|0)|0;z=I;Pn(M|0,z|0,B|0)|0;c[z>>2]=c[n>>2];c[z+4>>2]=c[n+4>>2];c[z+8>>2]=c[n+8>>2];c[z+12>>2]=c[n+12>>2];c[z+16>>2]=c[n+16>>2];R=M;break}M=t;z=(M-I|0)/20|0;B=H;A=(B-M|0)/20|0;if((z|0)==(A|0)){M=I;y=t;while(1){D=M;c[p>>2]=c[D>>2];c[p+4>>2]=c[D+4>>2];c[p+8>>2]=c[D+8>>2];c[p+12>>2]=c[D+12>>2];c[p+16>>2]=c[D+16>>2];S=y;c[D>>2]=c[S>>2];c[D+4>>2]=c[S+4>>2];c[D+8>>2]=c[S+8>>2];c[D+12>>2]=c[S+12>>2];c[D+16>>2]=c[S+16>>2];c[S>>2]=c[p>>2];c[S+4>>2]=c[p+4>>2];c[S+8>>2]=c[p+8>>2];c[S+12>>2]=c[p+12>>2];c[S+16>>2]=c[p+16>>2];S=M+20|0;if((S|0)==(t|0)){R=t;break d}else{M=S;y=y+20|0}}}else{T=z;U=A}while(1){y=(T|0)%(U|0)|0;if((y|0)==0){break}else{T=U;U=y}}if((U|0)!=0){y=z-1|0;M=I+(U*20|0)|0;while(1){N=M-20|0;S=N;c[o>>2]=c[S>>2];c[o+4>>2]=c[S+4>>2];c[o+8>>2]=c[S+8>>2];c[o+12>>2]=c[S+12>>2];c[o+16>>2]=c[S+16>>2];S=M+(y*20|0)|0;D=N;while(1){V=D;W=S;c[V>>2]=c[W>>2];c[V+4>>2]=c[W+4>>2];c[V+8>>2]=c[W+8>>2];c[V+12>>2]=c[W+12>>2];c[V+16>>2]=c[W+16>>2];V=(B-S|0)/20|0;if((z|0)<(V|0)){X=S+(z*20|0)|0}else{X=I+((z-V|0)*20|0)|0}if((X|0)==(N|0)){break}else{D=S;S=X}}c[W>>2]=c[o>>2];c[W+4>>2]=c[o+4>>2];c[W+8>>2]=c[o+8>>2];c[W+12>>2]=c[o+12>>2];c[W+16>>2]=c[o+16>>2];if((N|0)==(I|0)){break}else{M=N}}}R=I+(A*20|0)|0}}while(0);if((F+G|0)>=(Q+P|0)){break}bm(x,I,R,e,G,F,h,j);if((r|0)==(F|0)){v=45;break a}else{r=Q;s=P;t=H;u=R}}bm(R,H,f,e,P,Q,h,j);if((F|0)==0){v=45;break}else{q=F;g=G;f=R;d=I;b=x}}if((v|0)==8){cm(x,t,f,e,w,r,h);i=k;return}else if((v|0)==17){h=l;l=x;c[h>>2]=c[l>>2];c[h+4>>2]=c[l+4>>2];c[h+8>>2]=c[l+8>>2];c[h+12>>2]=c[l+12>>2];c[h+16>>2]=c[l+16>>2];x=t;c[l>>2]=c[x>>2];c[l+4>>2]=c[x+4>>2];c[l+8>>2]=c[x+8>>2];c[l+12>>2]=c[x+12>>2];c[l+16>>2]=c[x+16>>2];c[x>>2]=c[h>>2];c[x+4>>2]=c[h+4>>2];c[x+8>>2]=c[h+8>>2];c[x+12>>2]=c[h+12>>2];c[x+16>>2]=c[h+16>>2];i=k;return}else if((v|0)==45){i=k;return}}function cm(a,b,d,e,f,g,h){a=a|0;b=b|0;d=d|0;e=e|0;f=f|0;g=g|0;h=h|0;var j=0,k=0,l=0,m=0,n=0,o=0,p=0,q=0,r=0,s=0,t=0,u=0,v=0,w=0,x=0,y=0,z=0,A=0,B=0,C=0,D=0,E=0,F=0,G=0,H=0;j=i;k=a;l=b;m=i;i=i+8|0;n=m;o=i;i=i+8|0;p=o;q=i;i=i+8|0;r=q;s=i;i=i+8|0;t=s;u=i;i=i+8|0;v=i;i=i+4|0;i=i+7&-8;w=i;i=i+8|0;if((f|0)>(g|0)){if((b|0)==(d|0)){x=h}else{g=(((d-20+(-l|0)|0)>>>0)/20|0)+1|0;f=h;y=b;while(1){if((f|0)!=0){z=f;A=y;c[z>>2]=c[A>>2];c[z+4>>2]=c[A+4>>2];c[z+8>>2]=c[A+8>>2];c[z+12>>2]=c[A+12>>2];c[z+16>>2]=c[A+16>>2]}A=y+20|0;if((A|0)==(d|0)){break}else{f=f+20|0;y=A}}x=h+(g*20|0)|0}g=l;c[m>>2]=g;c[m+4>>2]=g;g=k;c[o>>2]=g;c[o+4>>2]=g;g=x;c[q>>2]=g;c[q+4>>2]=g;g=h;c[s>>2]=g;c[s+4>>2]=g;c[u>>2]=d;c[u+4>>2]=d;c[v>>2]=e;dm(w,n,p,r,t,u,v);i=j;return}a:do{if((a|0)==(b|0)){B=a;C=b}else{v=(((b-20+(-k|0)|0)>>>0)/20|0)+1|0;u=a;t=h;while(1){if((t|0)!=0){r=t;p=u;c[r>>2]=c[p>>2];c[r+4>>2]=c[p+4>>2];c[r+8>>2]=c[p+8>>2];c[r+12>>2]=c[p+12>>2];c[r+16>>2]=c[p+16>>2]}p=u+20|0;if((p|0)==(b|0)){break}else{u=p;t=t+20|0}}t=h+(v*20|0)|0;if((t|0)==(h|0)){B=a;C=b;break}else{D=a;E=h;F=b}while(1){if((F|0)==(d|0)){break}u=D;if(tb[c[e>>2]&63](F,E)|0){p=F;c[u>>2]=c[p>>2];c[u+4>>2]=c[p+4>>2];c[u+8>>2]=c[p+8>>2];c[u+12>>2]=c[p+12>>2];c[u+16>>2]=c[p+16>>2];G=E;H=F+20|0}else{p=E;c[u>>2]=c[p>>2];c[u+4>>2]=c[p+4>>2];c[u+8>>2]=c[p+8>>2];c[u+12>>2]=c[p+12>>2];c[u+16>>2]=c[p+16>>2];G=E+20|0;H=F}p=D+20|0;if((G|0)==(t|0)){B=p;C=H;break a}else{D=p;E=G;F=H}}Pn(D|0,E|0,t-E|0)|0;i=j;return}}while(0);Pn(B|0,C|0,d-C|0)|0;i=j;return}function dm(a,b,d,e,f,g,h){a=a|0;b=b|0;d=d|0;e=e|0;f=f|0;g=g|0;h=h|0;var j=0,k=0,l=0,m=0,n=0,o=0,p=0,q=0,r=0,s=0,t=0,u=0,v=0,w=0,x=0,y=0,z=0,A=0,B=0,C=0,D=0,E=0,F=0,G=0,H=0,I=0,J=0;j=i;k=b;b=i;i=i+8|0;c[b>>2]=c[k>>2];c[b+4>>2]=c[k+4>>2];k=d;d=i;i=i+8|0;c[d>>2]=c[k>>2];c[d+4>>2]=c[k+4>>2];k=e;e=i;i=i+8|0;c[e>>2]=c[k>>2];c[e+4>>2]=c[k+4>>2];k=f;f=i;i=i+8|0;c[f>>2]=c[k>>2];c[f+4>>2]=c[k+4>>2];k=g;g=i;i=i+8|0;c[g>>2]=c[k>>2];c[g+4>>2]=c[k+4>>2];k=h;h=i;i=i+4|0;i=i+7&-8;c[h>>2]=c[k>>2];k=b;l=c[k+4>>2]|0;m=c[d+4>>2]|0;d=m;n=e;o=c[n+4>>2]|0;p=f;a:do{if((l|0)==(d|0)){q=o;r=c[p+4>>2]|0}else{f=e+4|0;s=e|0;t=b+4|0;u=b|0;v=c[h>>2]|0;w=g+4|0;x=g|0;y=c[p+4>>2]|0;z=l;A=o;while(1){if((A|0)==(y|0)){break}B=(c[f>>2]|0)-20|0;c[s>>2]=B;C=(c[t>>2]|0)-20|0;c[u>>2]=C;D=tb[c[v>>2]&63](B,C)|0;E=(c[w>>2]|0)-20|0;c[x>>2]=E;if(D){c[u>>2]=C;D=E;F=C;c[D>>2]=c[F>>2];c[D+4>>2]=c[F+4>>2];c[D+8>>2]=c[F+8>>2];c[D+12>>2]=c[F+12>>2];c[D+16>>2]=c[F+16>>2];c[t>>2]=C}else{c[s>>2]=B;C=E;F=B;c[C>>2]=c[F>>2];c[C+4>>2]=c[F+4>>2];c[C+8>>2]=c[F+8>>2];c[C+12>>2]=c[F+12>>2];c[C+16>>2]=c[F+16>>2];c[f>>2]=B}c[w>>2]=E;E=c[k+4>>2]|0;B=c[n+4>>2]|0;if((E|0)==(d|0)){q=B;r=y;break a}else{z=E;A=B}}A=g;y=c[A+4>>2]|0;if((z|0)==(d|0)){G=c[A>>2]|0;H=y}else{A=((z-20+(-m|0)|0)>>>0)/20|0;w=y;f=z;do{w=w-20|0;f=f-20|0;s=w;t=f;c[s>>2]=c[t>>2];c[s+4>>2]=c[t+4>>2];c[s+8>>2]=c[t+8>>2];c[s+12>>2]=c[t+12>>2];c[s+16>>2]=c[t+16>>2];}while((f|0)!=(d|0));f=y+(~A*20|0)|0;G=f;H=f}f=a;c[f>>2]=G;c[f+4>>2]=H;i=j;return}}while(0);H=g;g=c[H+4>>2]|0;G=q;q=r;if((G|0)==(q|0)){I=c[H>>2]|0;J=g}else{H=((G-20+(-r|0)|0)>>>0)/20|0;r=g;d=G;do{r=r-20|0;d=d-20|0;G=r;m=d;c[G>>2]=c[m>>2];c[G+4>>2]=c[m+4>>2];c[G+8>>2]=c[m+8>>2];c[G+12>>2]=c[m+12>>2];c[G+16>>2]=c[m+16>>2];}while((d|0)!=(q|0));q=g+(~H*20|0)|0;I=q;J=q}q=a;c[q>>2]=I;c[q+4>>2]=J;i=j;return}function em(a){a=a|0;var b=0,d=0,e=0,f=0,g=0,h=0,i=0,j=0,k=0,l=0,m=0;b=a|0;d=a|0;e=a+4|0;f=c[e>>2]|0;if((f|0)!=(d|0)){g=f;do{f=hn(g)|0;h=c[f>>2]|0;if((h|0)!=0){i=f+32|0;j=0;while(1){k=i|0;l=i+4|0;m=i|0;c[(c[m>>2]|0)+4>>2]=c[l>>2];c[c[l>>2]>>2]=c[m>>2];c[l>>2]=k;c[m>>2]=k;k=j+1|0;if(k>>>0<h>>>0){i=i+12|0;j=k}else{break}}}kn(b,f);g=c[e>>2]|0;}while((g|0)!=(d|0))}g=a+12|0;j=a+20|0;i=a+24|0;h=j|0;c[(c[h>>2]|0)+4>>2]=c[i>>2];c[c[i>>2]>>2]=c[h>>2];c[i>>2]=j;c[h>>2]=j;j=g|0;h=a+16|0;i=g|0;c[(c[i>>2]|0)+4>>2]=c[h>>2];c[c[h>>2]>>2]=c[i>>2];c[h>>2]=j;c[i>>2]=j;ln(b);b=a|0;c[(c[b>>2]|0)+4>>2]=c[e>>2];c[c[e>>2]>>2]=c[b>>2];c[e>>2]=d;c[b>>2]=d;return}function fm(a){a=a|0;return}function gm(a){a=a|0;Jn(a);return}function hm(b,d){b=b|0;d=d|0;var e=0,f=0,g=0,h=0,j=0,k=0,l=0,m=0,n=0,o=0,p=0,q=0,r=0,s=0,t=0,u=0,v=0,w=0,x=0,y=0;e=i;i=i+40|0;f=e|0;g=e+16|0;if((a[d+38|0]|0)!=0){i=e;return 1}h=c[d+12>>2]|0;j=mb[c[(c[h>>2]|0)+12>>2]&15](h)|0;if((j|0)<=0){i=e;return 1}h=d+24|0;k=f;l=b+4|0;m=g+16|0;n=g+20|0;o=g|0;p=g+4|0;q=b;r=0;do{s=(c[h>>2]|0)+(r*28|0)|0;c[k>>2]=c[s>>2];c[k+4>>2]=c[s+4>>2];c[k+8>>2]=c[s+8>>2];c[k+12>>2]=c[s+12>>2];al(g,c[l>>2]|0,f);s=c[m>>2]|0;t=c[n>>2]|0;a:do{if(s>>>0<t>>>0){u=c[o>>2]|0;v=c[p>>2]|0;w=s;while(1){x=c[w+4>>2]&1048575;y=w+8|0;c[m>>2]=y;if(x>>>0<u>>>0|x>>>0>v>>>0){if(y>>>0<t>>>0){w=y;continue}else{break}}x=c[w>>2]|0;if(!((x|0)>-1)){break a}wb[c[(c[q>>2]|0)+20>>2]&63](b,d,r,x);if(y>>>0<t>>>0){w=y}else{break}}}}while(0);r=r+1|0;}while((r|0)<(j|0));i=e;return 1}function im(a,b){a=a|0;b=b|0;return 0}function jm(b,d,e,f){b=b|0;d=d|0;e=e|0;f=f|0;var h=0,j=0,l=0,m=0,n=0,o=0,p=0,q=0,r=0,s=0.0,t=0.0,u=0,v=0.0,w=0.0,x=0.0,y=0.0,z=0.0,A=0.0,B=0.0,C=0.0,D=0.0,E=0.0,F=0.0,G=0.0,H=0.0,I=0.0,J=0.0,K=0.0,L=0;h=i;i=i+40|0;j=h|0;l=h+16|0;m=c[d+8>>2]|0;n=b+4|0;o=c[n>>2]|0;p=(c[o+96>>2]|0)+(f<<3)|0;q=c[p>>2]|0;r=c[p+4>>2]|0;s=(c[k>>2]=q,+g[k>>2]);p=r;t=(c[k>>2]=p,+g[k>>2]);u=(c[o+104>>2]|0)+(f<<3)|0;v=+g[u>>2];w=+g[u+4>>2];if((c[o+24>>2]|0)==0){x=s- +g[m+28>>2];y=t- +g[m+32>>2];z=+g[m+40>>2];A=+g[m+36>>2];B=x*z+y*A;C=z*y+x*(-0.0-A);o=c[d+12>>2]|0;if((c[o+4>>2]|0)==0){x=+g[m+44>>2];y=B-x;D=+g[m+48>>2];E=C-D;F=y*z-E*A;G=z*E+y*A;A=+g[m+24>>2];y=+g[m+20>>2];H=x+(A*F+y*G);I=D+(A*G+F*(-0.0-y));J=A;K=y}else{H=B;I=C;J=+g[m+24>>2];K=+g[m+20>>2]}u=l;C=+(+g[m+12>>2]+(H*J-I*K));B=+(I*J+H*K+ +g[m+16>>2]);g[u>>2]=C;g[u+4>>2]=B;L=o}else{o=l;c[o>>2]=q|0;c[o+4>>2]=p|r&0;L=c[d+12>>2]|0}d=l+8|0;B=+g[b+8>>2];r=d;C=+(s+v*B);K=+(t+w*B);g[r>>2]=C;g[r+4>>2]=K;g[l+16>>2]=1.0;if(!(ub[c[(c[L>>2]|0)+24>>2]&31](L,j,l,m+12|0,e)|0)){i=h;return}e=j;K=+g[e>>2];C=+g[j+8>>2];B=1.0-C;j=b+12|0;H=+g[j>>2];J=H*(K*.004999999888241291+(+g[l>>2]*B+C*+g[d>>2])-s);s=H*(+g[e+4>>2]*.004999999888241291+(B*+g[l+4>>2]+C*+g[l+12>>2])-t);l=(c[(c[n>>2]|0)+104>>2]|0)+(f<<3)|0;t=+J;C=+s;g[l>>2]=t;g[l+4>>2]=C;l=c[n>>2]|0;C=+g[l+32>>2]*.75;t=+g[j>>2]*C*+g[l+320>>2]*C;C=(v-J)*t;J=(w-s)*t;if(!(C!=0.0|J!=0.0)){i=h;return}if((c[(c[l+88>>2]|0)+(f<<2)>>2]&4|0)!=0){i=h;return}j=l+21|0;n=l+112|0;if((a[j]|0)==0){Nn(c[n>>2]|0,0,c[l+44>>2]<<3|0)|0;a[j]=1}j=c[n>>2]|0;n=j+(f<<3)|0;g[n>>2]=C+ +g[n>>2];n=j+(f<<3)+4|0;g[n>>2]=J+ +g[n>>2];i=h;return}function km(a){a=a|0;return}function lm(a){a=a|0;Jn(a);return}function mm(b,d,e,f){b=b|0;d=d|0;e=e|0;f=f|0;var h=0,j=0,k=0,l=0,m=0,n=0,o=0,p=0,q=0.0,r=0.0,s=0.0,t=0.0,u=0.0,v=0.0,w=0.0,x=0.0,y=0,z=0,A=0,B=0,C=0,D=0,E=0,F=0,G=0,H=0.0,I=0,J=0;h=i;i=i+8|0;j=h|0;k=j;l=i;i=i+4|0;i=i+7&-8;m=i;i=i+8|0;n=b+4|0;o=(c[(c[n>>2]|0)+96>>2]|0)+(f<<3)|0;p=c[o+4>>2]|0;c[j>>2]=c[o>>2];c[j+4>>2]=p;p=c[d+12>>2]|0;o=d+8|0;sb[c[(c[p>>2]|0)+20>>2]&31](p,(c[o>>2]|0)+12|0,k,l,m,e);e=c[n>>2]|0;if(!(+g[l>>2]<+g[e+32>>2])){i=h;return}p=c[b+12>>2]|0;do{if((p|0)!=0){if((c[(c[e+88>>2]|0)+(f<<2)>>2]&65536|0)==0){break}if(qb[c[(c[p>>2]|0)+12>>2]&15](p,d,e,f)|0){break}i=h;return}}while(0);e=c[o>>2]|0;o=e+60|0;q=+g[o>>2];r=+g[e+132>>2];s=+g[e+44>>2];t=+g[e+48>>2];u=r*(s*s+t*t);t=+g[e+140>>2]+u-u;if(r>0.0){v=1.0/r}else{v=0.0}if(t>0.0){w=1.0/t}else{w=0.0}p=c[n>>2]|0;if((c[(c[p+88>>2]|0)+(f<<2)>>2]&4|0)==0){t=+g[p+36>>2]*1.3333333730697632;x=t*+g[p+28>>2]*t}else{x=0.0}y=m+4|0;z=m|0;t=(+g[j>>2]-q)*+g[y>>2]-(+g[k+4>>2]- +g[o+4>>2])*+g[z>>2];q=v+x+t*w*t;o=p+232|0;k=p+236|0;j=c[k>>2]|0;m=p+240|0;A=c[m>>2]|0;if((j|0)<(A|0)){B=j;C=c[o>>2]|0}else{D=(j|0)==0?256:j<<1;j=p+244|0;p=Vm(c[j>>2]|0,D*28|0)|0;E=p;F=o|0;o=c[F>>2]|0;if((o|0)!=0){G=A*28|0;On(p|0,o|0,G)|0;Wm(c[j>>2]|0,c[F>>2]|0,G)}c[m>>2]=D;c[F>>2]=E;B=c[k>>2]|0;C=E}c[k>>2]=B+1;k=C+(B*28|0)|0;c[k>>2]=f;c[C+(B*28|0)+4>>2]=e;c[C+(B*28|0)+8>>2]=d;g[C+(B*28|0)+12>>2]=1.0- +g[l>>2]*+g[(c[n>>2]|0)+36>>2];l=C+(B*28|0)+16|0;t=+(-0.0- +g[z>>2]);w=+(-0.0- +g[y>>2]);g[l>>2]=t;g[l+4>>2]=w;if(q>0.0){H=1.0/q}else{H=0.0}g[C+(B*28|0)+24>>2]=H;B=c[b+16>>2]|0;a:do{if((B|0)!=0){C=c[b+20>>2]|0;l=c[C+8>>2]|0;do{if((l|0)!=0){y=c[C>>2]|0;z=y;e=y;y=z+(l<<3)-e>>3;b:do{if((y|0)==0){I=z}else{E=y;F=z;while(1){D=E;while(1){J=(D|0)/2|0;if((c[F+(J<<3)>>2]|0)>>>0<d>>>0){if((c[F+(J<<3)+4>>2]|0)<(f|0)){break}}if((D+1|0)>>>0<3>>>0){I=F;break b}else{D=J}}m=F+(J+1<<3)|0;G=D-1|0;if((G|0)==(J|0)){I=m;break}else{E=G-J|0;F=m}}}}while(0);if((I|0)==0){break}z=(c[C+4>>2]|0)+((I-e|0)>>>3)|0;if((a[z]|0)==0){break}a[z]=0;break a}}while(0);ob[c[(c[B>>2]|0)+16>>2]&31](B,c[n>>2]|0,k)}}while(0);fl(c[n>>2]|0,f);i=h;return}function nm(a,b){a=a|0;b=b|0;var d=0;if(!((c[a>>2]|0)>>>0<(c[b>>2]|0)>>>0)){d=0;return d|0}d=(c[a+4>>2]|0)<(c[b+4>>2]|0);return d|0}function om(a,b,d){a=a|0;b=b|0;d=d|0;var e=0,f=0,g=0,h=0,j=0,k=0,l=0,m=0,n=0,o=0,p=0,q=0,r=0,s=0,t=0,u=0,v=0,w=0,x=0,y=0,z=0,A=0,B=0,C=0,D=0,E=0,F=0,G=0,H=0,I=0,J=0,K=0,L=0,M=0,N=0,O=0,P=0,Q=0,R=0,S=0;e=i;i=i+8|0;f=e|0;g=a;a=b;a:while(1){b=a;h=a-8|0;j=h;k=g;b:while(1){l=k;m=b-l|0;n=m>>3;switch(n|0){case 5:{o=26;break a;break};case 4:{o=14;break a;break};case 2:{o=4;break a;break};case 3:{o=6;break a;break};case 0:case 1:{o=83;break a;break};default:{}}if((m|0)<248){o=28;break a}p=(n|0)/2|0;q=k+(p<<3)|0;do{if((m|0)>7992){r=(n|0)/4|0;s=pm(k,k+(r<<3)|0,q,k+(r+p<<3)|0,h,d)|0}else{r=tb[c[d>>2]&63](q,k)|0;t=tb[c[d>>2]&63](h,q)|0;if(!r){if(!t){s=0;break}r=q;u=c[r>>2]|0;v=c[r+4>>2]|0;w=c[j+4>>2]|0;c[r>>2]=c[j>>2];c[r+4>>2]=w;c[j>>2]=u;c[j+4>>2]=v;if(!(tb[c[d>>2]&63](q,k)|0)){s=1;break}v=k;u=c[v>>2]|0;w=c[v+4>>2]|0;x=c[r+4>>2]|0;c[v>>2]=c[r>>2];c[v+4>>2]=x;c[r>>2]=u;c[r+4>>2]=w;s=2;break}w=k;r=c[w>>2]|0;u=c[w+4>>2]|0;if(t){t=c[j+4>>2]|0;c[w>>2]=c[j>>2];c[w+4>>2]=t;c[j>>2]=r;c[j+4>>2]=u;s=1;break}t=q;x=c[t+4>>2]|0;c[w>>2]=c[t>>2];c[w+4>>2]=x;c[t>>2]=r;c[t+4>>2]=u;if(!(tb[c[d>>2]&63](h,q)|0)){s=1;break}u=c[t>>2]|0;r=c[t+4>>2]|0;x=c[j+4>>2]|0;c[t>>2]=c[j>>2];c[t+4>>2]=x;c[j>>2]=u;c[j+4>>2]=r;s=2}}while(0);do{if(tb[c[d>>2]&63](k,q)|0){y=h;z=s}else{p=h;while(1){A=p-8|0;if((k|0)==(A|0)){break}if(tb[c[d>>2]&63](A,q)|0){o=66;break}else{p=A}}if((o|0)==66){o=0;p=k;n=c[p>>2]|0;m=c[p+4>>2]|0;r=A;u=c[r+4>>2]|0;c[p>>2]=c[r>>2];c[p+4>>2]=u;c[r>>2]=n;c[r+4>>2]=m;y=A;z=s+1|0;break}m=k+8|0;if(tb[c[d>>2]&63](k,h)|0){B=m}else{r=m;while(1){if((r|0)==(h|0)){o=83;break a}C=r+8|0;if(tb[c[d>>2]&63](k,r)|0){break}else{r=C}}m=r;n=c[m>>2]|0;u=c[m+4>>2]|0;p=c[j+4>>2]|0;c[m>>2]=c[j>>2];c[m+4>>2]=p;c[j>>2]=n;c[j+4>>2]=u;B=C}if((B|0)==(h|0)){o=83;break a}else{D=h;E=B}while(1){u=E;while(1){F=u+8|0;if(tb[c[d>>2]&63](k,u)|0){G=D;break}else{u=F}}do{G=G-8|0;}while(tb[c[d>>2]&63](k,G)|0);if(!(u>>>0<G>>>0)){k=u;continue b}n=u;p=c[n>>2]|0;m=c[n+4>>2]|0;x=G;t=c[x+4>>2]|0;c[n>>2]=c[x>>2];c[n+4>>2]=t;c[x>>2]=p;c[x+4>>2]=m;D=G;E=F}}}while(0);r=k+8|0;c:do{if(r>>>0<y>>>0){m=y;x=r;p=z;t=q;while(1){n=x;while(1){H=n+8|0;if(tb[c[d>>2]&63](n,t)|0){n=H}else{I=m;break}}do{I=I-8|0;}while(!(tb[c[d>>2]&63](I,t)|0));if(n>>>0>I>>>0){J=n;K=p;L=t;break c}u=n;w=c[u>>2]|0;v=c[u+4>>2]|0;M=I;N=c[M+4>>2]|0;c[u>>2]=c[M>>2];c[u+4>>2]=N;c[M>>2]=w;c[M+4>>2]=v;m=I;x=H;p=p+1|0;t=(t|0)==(n|0)?I:t}}else{J=r;K=z;L=q}}while(0);do{if((J|0)==(L|0)){O=K}else{if(!(tb[c[d>>2]&63](L,J)|0)){O=K;break}q=J;r=c[q>>2]|0;t=c[q+4>>2]|0;p=L;x=c[p+4>>2]|0;c[q>>2]=c[p>>2];c[q+4>>2]=x;c[p>>2]=r;c[p+4>>2]=t;O=K+1|0}}while(0);if((O|0)==0){P=qm(k,J,d)|0;t=J+8|0;if(qm(t,a,d)|0){o=78;break}if(P){k=t;continue}}t=J;if((t-l|0)>=(b-t|0)){o=82;break}om(k,J,d);k=J+8|0}if((o|0)==78){o=0;if(P){o=83;break}else{g=k;a=J;continue}}else if((o|0)==82){o=0;om(J+8|0,a,d);g=k;a=J;continue}}if((o|0)==4){if(!(tb[c[d>>2]&63](h,k)|0)){i=e;return}J=k;g=c[J>>2]|0;P=c[J+4>>2]|0;O=c[j+4>>2]|0;c[J>>2]=c[j>>2];c[J+4>>2]=O;c[j>>2]=g;c[j+4>>2]=P;i=e;return}else if((o|0)==6){P=k+8|0;g=tb[c[d>>2]&63](P,k)|0;O=tb[c[d>>2]&63](h,P)|0;if(!g){if(!O){i=e;return}g=P;J=c[g>>2]|0;K=c[g+4>>2]|0;L=c[j+4>>2]|0;c[g>>2]=c[j>>2];c[g+4>>2]=L;c[j>>2]=J;c[j+4>>2]=K;if(!(tb[c[d>>2]&63](P,k)|0)){i=e;return}K=k;J=c[K>>2]|0;L=c[K+4>>2]|0;z=c[g+4>>2]|0;c[K>>2]=c[g>>2];c[K+4>>2]=z;c[g>>2]=J;c[g+4>>2]=L;i=e;return}L=k;g=c[L>>2]|0;J=c[L+4>>2]|0;if(O){O=c[j+4>>2]|0;c[L>>2]=c[j>>2];c[L+4>>2]=O;c[j>>2]=g;c[j+4>>2]=J;i=e;return}O=P;z=c[O+4>>2]|0;c[L>>2]=c[O>>2];c[L+4>>2]=z;c[O>>2]=g;c[O+4>>2]=J;if(!(tb[c[d>>2]&63](h,P)|0)){i=e;return}P=c[O>>2]|0;J=c[O+4>>2]|0;g=c[j+4>>2]|0;c[O>>2]=c[j>>2];c[O+4>>2]=g;c[j>>2]=P;c[j+4>>2]=J;i=e;return}else if((o|0)==14){J=k+8|0;P=k+16|0;g=tb[c[d>>2]&63](J,k)|0;O=tb[c[d>>2]&63](P,J)|0;do{if(g){z=k;L=c[z>>2]|0;K=c[z+4>>2]|0;if(O){I=P;H=c[I+4>>2]|0;c[z>>2]=c[I>>2];c[z+4>>2]=H;c[I>>2]=L;c[I+4>>2]=K;break}I=J;H=c[I+4>>2]|0;c[z>>2]=c[I>>2];c[z+4>>2]=H;c[I>>2]=L;c[I+4>>2]=K;if(!(tb[c[d>>2]&63](P,J)|0)){break}K=c[I>>2]|0;L=c[I+4>>2]|0;H=P;z=c[H+4>>2]|0;c[I>>2]=c[H>>2];c[I+4>>2]=z;c[H>>2]=K;c[H+4>>2]=L}else{if(!O){break}L=J;H=c[L>>2]|0;K=c[L+4>>2]|0;z=P;I=c[z+4>>2]|0;c[L>>2]=c[z>>2];c[L+4>>2]=I;c[z>>2]=H;c[z+4>>2]=K;if(!(tb[c[d>>2]&63](J,k)|0)){break}K=k;z=c[K>>2]|0;H=c[K+4>>2]|0;I=c[L+4>>2]|0;c[K>>2]=c[L>>2];c[K+4>>2]=I;c[L>>2]=z;c[L+4>>2]=H}}while(0);if(!(tb[c[d>>2]&63](h,P)|0)){i=e;return}O=P;g=c[O>>2]|0;H=c[O+4>>2]|0;L=c[j+4>>2]|0;c[O>>2]=c[j>>2];c[O+4>>2]=L;c[j>>2]=g;c[j+4>>2]=H;if(!(tb[c[d>>2]&63](P,J)|0)){i=e;return}P=J;H=c[P>>2]|0;j=c[P+4>>2]|0;g=c[O+4>>2]|0;c[P>>2]=c[O>>2];c[P+4>>2]=g;c[O>>2]=H;c[O+4>>2]=j;if(!(tb[c[d>>2]&63](J,k)|0)){i=e;return}J=k;j=c[J>>2]|0;O=c[J+4>>2]|0;H=c[P+4>>2]|0;c[J>>2]=c[P>>2];c[J+4>>2]=H;c[P>>2]=j;c[P+4>>2]=O;i=e;return}else if((o|0)==26){pm(k,k+8|0,k+16|0,k+24|0,h,d)|0;i=e;return}else if((o|0)==28){h=f;O=k+16|0;P=k+8|0;j=tb[c[d>>2]&63](P,k)|0;H=tb[c[d>>2]&63](O,P)|0;do{if(j){J=k;g=c[J>>2]|0;L=c[J+4>>2]|0;if(H){z=O;I=c[z+4>>2]|0;c[J>>2]=c[z>>2];c[J+4>>2]=I;c[z>>2]=g;c[z+4>>2]=L;break}z=P;I=c[z+4>>2]|0;c[J>>2]=c[z>>2];c[J+4>>2]=I;c[z>>2]=g;c[z+4>>2]=L;if(!(tb[c[d>>2]&63](O,P)|0)){break}L=c[z>>2]|0;g=c[z+4>>2]|0;I=O;J=c[I+4>>2]|0;c[z>>2]=c[I>>2];c[z+4>>2]=J;c[I>>2]=L;c[I+4>>2]=g}else{if(!H){break}g=P;I=c[g>>2]|0;L=c[g+4>>2]|0;J=O;z=c[J+4>>2]|0;c[g>>2]=c[J>>2];c[g+4>>2]=z;c[J>>2]=I;c[J+4>>2]=L;if(!(tb[c[d>>2]&63](P,k)|0)){break}L=k;J=c[L>>2]|0;I=c[L+4>>2]|0;z=c[g+4>>2]|0;c[L>>2]=c[g>>2];c[L+4>>2]=z;c[g>>2]=J;c[g+4>>2]=I}}while(0);P=k+24|0;if((P|0)==(a|0)){i=e;return}else{Q=O;R=P}while(1){if(tb[c[d>>2]&63](R,Q)|0){P=R;O=c[P+4>>2]|0;c[f>>2]=c[P>>2];c[f+4>>2]=O;O=Q;P=R;while(1){S=O;H=P;j=c[S+4>>2]|0;c[H>>2]=c[S>>2];c[H+4>>2]=j;if((O|0)==(k|0)){break}j=O-8|0;if(tb[c[d>>2]&63](h,j)|0){P=O;O=j}else{break}}O=c[f+4>>2]|0;c[S>>2]=c[f>>2];c[S+4>>2]=O}O=R+8|0;if((O|0)==(a|0)){break}else{Q=R;R=O}}i=e;return}else if((o|0)==83){i=e;return}}function pm(a,b,d,e,f,g){a=a|0;b=b|0;d=d|0;e=e|0;f=f|0;g=g|0;var h=0,i=0,j=0,k=0,l=0,m=0,n=0,o=0,p=0,q=0;h=tb[c[g>>2]&63](b,a)|0;i=tb[c[g>>2]&63](d,b)|0;do{if(h){j=a;k=c[j>>2]|0;l=c[j+4>>2]|0;if(i){m=d;n=c[m+4>>2]|0;c[j>>2]=c[m>>2];c[j+4>>2]=n;c[m>>2]=k;c[m+4>>2]=l;o=1;break}m=b;n=c[m+4>>2]|0;c[j>>2]=c[m>>2];c[j+4>>2]=n;c[m>>2]=k;c[m+4>>2]=l;if(!(tb[c[g>>2]&63](d,b)|0)){o=1;break}l=c[m>>2]|0;k=c[m+4>>2]|0;n=d;j=c[n+4>>2]|0;c[m>>2]=c[n>>2];c[m+4>>2]=j;c[n>>2]=l;c[n+4>>2]=k;o=2}else{if(!i){o=0;break}k=b;n=c[k>>2]|0;l=c[k+4>>2]|0;j=d;m=c[j+4>>2]|0;c[k>>2]=c[j>>2];c[k+4>>2]=m;c[j>>2]=n;c[j+4>>2]=l;if(!(tb[c[g>>2]&63](b,a)|0)){o=1;break}l=a;j=c[l>>2]|0;n=c[l+4>>2]|0;m=c[k+4>>2]|0;c[l>>2]=c[k>>2];c[l+4>>2]=m;c[k>>2]=j;c[k+4>>2]=n;o=2}}while(0);do{if(tb[c[g>>2]&63](e,d)|0){i=d;h=c[i>>2]|0;n=c[i+4>>2]|0;k=e;j=c[k+4>>2]|0;c[i>>2]=c[k>>2];c[i+4>>2]=j;c[k>>2]=h;c[k+4>>2]=n;if(!(tb[c[g>>2]&63](d,b)|0)){p=o+1|0;break}n=b;k=c[n>>2]|0;h=c[n+4>>2]|0;j=c[i+4>>2]|0;c[n>>2]=c[i>>2];c[n+4>>2]=j;c[i>>2]=k;c[i+4>>2]=h;if(!(tb[c[g>>2]&63](b,a)|0)){p=o+2|0;break}h=a;i=c[h>>2]|0;k=c[h+4>>2]|0;j=c[n+4>>2]|0;c[h>>2]=c[n>>2];c[h+4>>2]=j;c[n>>2]=i;c[n+4>>2]=k;p=o+3|0}else{p=o}}while(0);if(!(tb[c[g>>2]&63](f,e)|0)){q=p;return q|0}o=e;k=c[o>>2]|0;n=c[o+4>>2]|0;i=f;f=c[i+4>>2]|0;c[o>>2]=c[i>>2];c[o+4>>2]=f;c[i>>2]=k;c[i+4>>2]=n;if(!(tb[c[g>>2]&63](e,d)|0)){q=p+1|0;return q|0}e=d;n=c[e>>2]|0;i=c[e+4>>2]|0;k=c[o+4>>2]|0;c[e>>2]=c[o>>2];c[e+4>>2]=k;c[o>>2]=n;c[o+4>>2]=i;if(!(tb[c[g>>2]&63](d,b)|0)){q=p+2|0;return q|0}d=b;i=c[d>>2]|0;o=c[d+4>>2]|0;n=c[e+4>>2]|0;c[d>>2]=c[e>>2];c[d+4>>2]=n;c[e>>2]=i;c[e+4>>2]=o;if(!(tb[c[g>>2]&63](b,a)|0)){q=p+3|0;return q|0}b=a;a=c[b>>2]|0;g=c[b+4>>2]|0;o=c[d+4>>2]|0;c[b>>2]=c[d>>2];c[b+4>>2]=o;c[d>>2]=a;c[d+4>>2]=g;q=p+4|0;return q|0}function qm(a,b,d){a=a|0;b=b|0;d=d|0;var e=0,f=0,g=0,h=0,j=0,k=0,l=0,m=0,n=0,o=0,p=0,q=0,r=0,s=0,t=0,u=0,v=0,w=0,x=0,y=0;e=i;i=i+8|0;f=e|0;g=f;switch(b-a>>3|0){case 2:{h=b-8|0;if(!(tb[c[d>>2]&63](h,a)|0)){j=1;i=e;return j|0}k=a;l=c[k>>2]|0;m=c[k+4>>2]|0;n=h;h=c[n+4>>2]|0;c[k>>2]=c[n>>2];c[k+4>>2]=h;c[n>>2]=l;c[n+4>>2]=m;j=1;i=e;return j|0};case 4:{m=a+8|0;n=a+16|0;l=b-8|0;h=tb[c[d>>2]&63](m,a)|0;k=tb[c[d>>2]&63](n,m)|0;do{if(h){o=a;p=c[o>>2]|0;q=c[o+4>>2]|0;if(k){r=n;s=c[r+4>>2]|0;c[o>>2]=c[r>>2];c[o+4>>2]=s;c[r>>2]=p;c[r+4>>2]=q;break}r=m;s=c[r+4>>2]|0;c[o>>2]=c[r>>2];c[o+4>>2]=s;c[r>>2]=p;c[r+4>>2]=q;if(!(tb[c[d>>2]&63](n,m)|0)){break}q=c[r>>2]|0;p=c[r+4>>2]|0;s=n;o=c[s+4>>2]|0;c[r>>2]=c[s>>2];c[r+4>>2]=o;c[s>>2]=q;c[s+4>>2]=p}else{if(!k){break}p=m;s=c[p>>2]|0;q=c[p+4>>2]|0;o=n;r=c[o+4>>2]|0;c[p>>2]=c[o>>2];c[p+4>>2]=r;c[o>>2]=s;c[o+4>>2]=q;if(!(tb[c[d>>2]&63](m,a)|0)){break}q=a;o=c[q>>2]|0;s=c[q+4>>2]|0;r=c[p+4>>2]|0;c[q>>2]=c[p>>2];c[q+4>>2]=r;c[p>>2]=o;c[p+4>>2]=s}}while(0);if(!(tb[c[d>>2]&63](l,n)|0)){j=1;i=e;return j|0}k=n;h=c[k>>2]|0;s=c[k+4>>2]|0;p=l;l=c[p+4>>2]|0;c[k>>2]=c[p>>2];c[k+4>>2]=l;c[p>>2]=h;c[p+4>>2]=s;if(!(tb[c[d>>2]&63](n,m)|0)){j=1;i=e;return j|0}n=m;s=c[n>>2]|0;p=c[n+4>>2]|0;h=c[k+4>>2]|0;c[n>>2]=c[k>>2];c[n+4>>2]=h;c[k>>2]=s;c[k+4>>2]=p;if(!(tb[c[d>>2]&63](m,a)|0)){j=1;i=e;return j|0}m=a;p=c[m>>2]|0;k=c[m+4>>2]|0;s=c[n+4>>2]|0;c[m>>2]=c[n>>2];c[m+4>>2]=s;c[n>>2]=p;c[n+4>>2]=k;j=1;i=e;return j|0};case 5:{pm(a,a+8|0,a+16|0,a+24|0,b-8|0,d)|0;j=1;i=e;return j|0};case 0:case 1:{j=1;i=e;return j|0};case 3:{k=a+8|0;n=b-8|0;p=tb[c[d>>2]&63](k,a)|0;s=tb[c[d>>2]&63](n,k)|0;if(!p){if(!s){j=1;i=e;return j|0}p=k;m=c[p>>2]|0;h=c[p+4>>2]|0;l=n;o=c[l+4>>2]|0;c[p>>2]=c[l>>2];c[p+4>>2]=o;c[l>>2]=m;c[l+4>>2]=h;if(!(tb[c[d>>2]&63](k,a)|0)){j=1;i=e;return j|0}h=a;l=c[h>>2]|0;m=c[h+4>>2]|0;o=c[p+4>>2]|0;c[h>>2]=c[p>>2];c[h+4>>2]=o;c[p>>2]=l;c[p+4>>2]=m;j=1;i=e;return j|0}m=a;p=c[m>>2]|0;l=c[m+4>>2]|0;if(s){s=n;o=c[s+4>>2]|0;c[m>>2]=c[s>>2];c[m+4>>2]=o;c[s>>2]=p;c[s+4>>2]=l;j=1;i=e;return j|0}s=k;o=c[s+4>>2]|0;c[m>>2]=c[s>>2];c[m+4>>2]=o;c[s>>2]=p;c[s+4>>2]=l;if(!(tb[c[d>>2]&63](n,k)|0)){j=1;i=e;return j|0}k=c[s>>2]|0;l=c[s+4>>2]|0;p=n;n=c[p+4>>2]|0;c[s>>2]=c[p>>2];c[s+4>>2]=n;c[p>>2]=k;c[p+4>>2]=l;j=1;i=e;return j|0};default:{l=a+16|0;p=a+8|0;k=tb[c[d>>2]&63](p,a)|0;n=tb[c[d>>2]&63](l,p)|0;do{if(k){s=a;o=c[s>>2]|0;m=c[s+4>>2]|0;if(n){h=l;r=c[h+4>>2]|0;c[s>>2]=c[h>>2];c[s+4>>2]=r;c[h>>2]=o;c[h+4>>2]=m;break}h=p;r=c[h+4>>2]|0;c[s>>2]=c[h>>2];c[s+4>>2]=r;c[h>>2]=o;c[h+4>>2]=m;if(!(tb[c[d>>2]&63](l,p)|0)){break}m=c[h>>2]|0;o=c[h+4>>2]|0;r=l;s=c[r+4>>2]|0;c[h>>2]=c[r>>2];c[h+4>>2]=s;c[r>>2]=m;c[r+4>>2]=o}else{if(!n){break}o=p;r=c[o>>2]|0;m=c[o+4>>2]|0;s=l;h=c[s+4>>2]|0;c[o>>2]=c[s>>2];c[o+4>>2]=h;c[s>>2]=r;c[s+4>>2]=m;if(!(tb[c[d>>2]&63](p,a)|0)){break}m=a;s=c[m>>2]|0;r=c[m+4>>2]|0;h=c[o+4>>2]|0;c[m>>2]=c[o>>2];c[m+4>>2]=h;c[o>>2]=s;c[o+4>>2]=r}}while(0);p=a+24|0;if((p|0)==(b|0)){j=1;i=e;return j|0}else{t=l;u=0;v=p}while(1){if(tb[c[d>>2]&63](v,t)|0){p=v;l=c[p+4>>2]|0;c[f>>2]=c[p>>2];c[f+4>>2]=l;l=t;p=v;while(1){w=l;n=p;k=c[w+4>>2]|0;c[n>>2]=c[w>>2];c[n+4>>2]=k;if((l|0)==(a|0)){break}k=l-8|0;if(tb[c[d>>2]&63](g,k)|0){p=l;l=k}else{break}}l=c[f+4>>2]|0;c[w>>2]=c[f>>2];c[w+4>>2]=l;l=u+1|0;if((l|0)==8){break}else{x=l}}else{x=u}l=v+8|0;if((l|0)==(b|0)){j=1;y=41;break}else{t=v;u=x;v=l}}if((y|0)==41){i=e;return j|0}j=(v+8|0)==(b|0);i=e;return j|0}}return 0}function rm(a){a=a|0;return}function sm(a){a=a|0;Jn(a);return}function tm(a,b,d,e){a=a|0;b=b|0;d=d|0;e=e|0;var f=0,h=0,i=0,j=0.0,k=0,l=0.0,m=0.0,n=0,o=0.0,p=0,q=0.0,r=0.0,s=0,t=0.0,u=0.0,v=0,w=0.0,x=0,y=0,z=0,A=0,B=0,C=0,D=0,E=0,F=0,G=0,H=0,I=0,J=0,K=0.0,L=0.0,M=0.0;f=a+4|0;h=c[(c[f>>2]|0)+88>>2]|0;i=c[h+(d<<2)>>2]|c[h+(b<<2)>>2]|c[h+(e<<2)>>2];if((i&16|0)==0){return}h=c[a+8>>2]|0;if(!(qb[c[(c[h>>2]|0)+16>>2]&15](h,b,d,e)|0)){return}h=c[f>>2]|0;f=c[h+96>>2]|0;a=f+(b<<3)|0;j=+g[a>>2];k=f+(d<<3)|0;l=+g[k>>2];m=j-l;n=f+(b<<3)+4|0;o=+g[n>>2];p=f+(d<<3)+4|0;q=+g[p>>2];r=o-q;s=f+(e<<3)|0;t=+g[s>>2];u=l-t;v=f+(e<<3)+4|0;l=+g[v>>2];w=q-l;q=t-j;j=l-o;o=+g[h+40>>2]*4.0;if(m*m+r*r>o){return}if(u*u+w*w>o){return}if(q*q+j*j>o){return}f=c[h+144>>2]|0;x=c[f+(b<<2)>>2]|0;y=c[f+(d<<2)>>2]|0;z=c[f+(e<<2)>>2]|0;f=h+264|0;A=h+268|0;B=c[A>>2]|0;C=h+272|0;D=c[C>>2]|0;if((B|0)<(D|0)){E=B;F=c[f>>2]|0}else{G=(B|0)==0?256:B<<1;B=h+276|0;h=Vm(c[B>>2]|0,G*60|0)|0;H=h;I=f|0;f=c[I>>2]|0;if((f|0)!=0){J=D*60|0;On(h|0,f|0,J)|0;Wm(c[B>>2]|0,c[I>>2]|0,J)}c[C>>2]=G;c[I>>2]=H;E=c[A>>2]|0;F=H}c[A>>2]=E+1;c[F+(E*60|0)>>2]=b;c[F+(E*60|0)+4>>2]=d;c[F+(E*60|0)+8>>2]=e;c[F+(E*60|0)+12>>2]=i;if((x|0)==0){K=1.0}else{K=+g[x+16>>2]}if((y|0)==0){L=1.0}else{L=+g[y+16>>2]}o=K<L?K:L;if((z|0)==0){M=1.0}else{M=+g[z+16>>2]}g[F+(E*60|0)+16>>2]=o<M?o:M;M=+g[a>>2];o=+g[n>>2];L=(M+ +g[k>>2]+ +g[s>>2])*.3333333432674408;K=(o+ +g[p>>2]+ +g[v>>2])*.3333333432674408;z=F+(E*60|0)+20|0;l=+(M-L);M=+(o-K);g[z>>2]=l;g[z+4>>2]=M;z=F+(E*60|0)+28|0;M=+(+g[k>>2]-L);l=+(+g[p>>2]-K);g[z>>2]=M;g[z+4>>2]=l;z=F+(E*60|0)+36|0;l=+(+g[s>>2]-L);L=+(+g[v>>2]-K);g[z>>2]=l;g[z+4>>2]=L;g[F+(E*60|0)+44>>2]=-0.0-(m*q+r*j);g[F+(E*60|0)+48>>2]=-0.0-(m*u+r*w);g[F+(E*60|0)+52>>2]=-0.0-(u*q+w*j);j=+g[a>>2];w=+g[p>>2];q=+g[n>>2];u=+g[k>>2];r=+g[v>>2];m=+g[s>>2];g[F+(E*60|0)+56>>2]=q*m-j*r+(j*w-q*u+(u*r-w*m));return}function um(a){a=a|0;return}function vm(a){a=a|0;Jn(a);return}function wm(a,b){a=a|0;b=b|0;return(c[(c[a+4>>2]|0)+(b<<2)>>2]&4096|0)!=0|0}function xm(a,b,c){a=a|0;b=b|0;c=c|0;return 1}function ym(a,b,c,d){a=a|0;b=b|0;c=c|0;d=d|0;return 1}function zm(a){a=a|0;Jn(a);return}function Am(a,b){a=a|0;b=b|0;return 1}function Bm(a){a=a|0;return}function Cm(a){a=a|0;Jn(a);return}function Dm(a,b,d){a=a|0;b=b|0;d=d|0;var e=0;e=c[a+4>>2]|0;a=(e|0)<=(b|0);b=(e|0)>(d|0);if(a|b){return b&a|0}else{return 1}return 0}function Em(a,b,d,e){a=a|0;b=b|0;d=d|0;e=e|0;var f=0;f=c[a+4>>2]|0;a=(f|0)>(b|0);b=(f|0)>(d|0);if(a|b|(f|0)>(e|0)){return(f|0)<=(e|0)|a&b^1|0}else{return 0}return 0}function Fm(a){a=a|0;return}function Gm(a){a=a|0;Jn(a);return}function Hm(a,b){a=a|0;b=b|0;return 0}function Im(a){a=a|0;return 1}function Jm(a,b,d){a=a|0;b=b|0;d=d|0;var e=0,f=0,g=0,h=0,i=0;e=a+16|0;f=a+12|0;a=0;while(1){if((a|0)>=(c[e>>2]|0)){g=0;h=4;break}i=c[(c[f>>2]|0)+(a<<2)>>2]|0;if(lb[c[(c[i>>2]|0)+16>>2]&31](i,b,d)|0){g=1;h=4;break}else{a=a+1|0}}if((h|0)==4){return g|0}return 0}function Km(a,b,c,d,e,f){a=a|0;b=b|0;c=c|0;d=d|0;e=e|0;f=f|0;return}function Lm(a,b,c,d,e){a=a|0;b=b|0;c=c|0;d=d|0;e=e|0;return 0}function Mm(a,b,d,e){a=a|0;b=b|0;d=d|0;e=e|0;var f=0,h=0,j=0,k=0,l=0,m=0,n=0,o=0,p=0,q=0,r=0,s=0,t=0,u=0,v=0,w=0.0,x=0.0,y=0.0,z=0.0,A=0.0;e=i;i=i+16|0;f=e|0;h=b|0;g[h>>2]=3.4028234663852886e+38;j=b+4|0;g[j>>2]=3.4028234663852886e+38;g[b+8>>2]=-3.4028234663852886e+38;k=b+12|0;g[k>>2]=-3.4028234663852886e+38;l=a+16|0;if((c[l>>2]|0)<=0){i=e;return}m=a+12|0;a=f|0;n=f+4|0;o=b;p=b+8|0;b=p|0;q=f+8|0;r=f+12|0;s=p;p=0;do{t=c[(c[m>>2]|0)+(p<<2)>>2]|0;u=mb[c[(c[t>>2]|0)+12>>2]&15](t)|0;if((u|0)>0){t=0;do{v=c[(c[m>>2]|0)+(p<<2)>>2]|0;wb[c[(c[v>>2]|0)+28>>2]&63](v,f,d,t);w=+g[h>>2];x=+g[a>>2];y=+g[j>>2];z=+g[n>>2];A=+(w<x?w:x);x=+(y<z?y:z);g[o>>2]=A;g[o+4>>2]=x;x=+g[b>>2];A=+g[q>>2];z=+g[k>>2];y=+g[r>>2];w=+(x>A?x:A);A=+(z>y?z:y);g[s>>2]=w;g[s+4>>2]=A;t=t+1|0;}while((t|0)<(u|0))}p=p+1|0;}while((p|0)<(c[l>>2]|0));i=e;return}function Nm(a,b,c){a=a|0;b=b|0;c=+c;return}function Om(a){a=a|0;return}function Pm(a){a=a|0;Jn(a);return}function Qm(a,b){a=a|0;b=b|0;return 0}function Rm(b,d,e){b=b|0;d=d|0;e=e|0;var f=0,g=0,h=0;f=b+4|0;if((c[f>>2]|0)!=(d|0)){g=0;return g|0}h=c[b+8>>2]|0;if(!(lb[c[(c[h>>2]|0)+16>>2]&31](h,b+12|0,(c[d+96>>2]|0)+(e<<3)|0)|0)){g=1;return g|0}d=c[f>>2]|0;Pk(d,e,c[(c[d+88>>2]|0)+(e<<2)>>2]|((a[b+28|0]|0)!=0?514:2));e=b+32|0;c[e>>2]=(c[e>>2]|0)+1;g=1;return g|0}function Sm(a,b){a=a|0;b=b|0;var d=0,e=0,f=0;d=b+4|0;e=b|0;c[(c[e>>2]|0)+4>>2]=c[d>>2];c[c[d>>2]>>2]=c[e>>2];c[d>>2]=b;c[e>>2]=b;f=a+12|0;c[d>>2]=c[f>>2];c[e>>2]=a+8;c[c[f>>2]>>2]=b;c[f>>2]=b;return}function Tm(b){b=b|0;var d=0,e=0,f=0,g=0,h=0;d=b+68|0;e=d|0;c[b+72>>2]=e;c[d>>2]=e;e=b+8|0;c[e>>2]=128;c[b+4>>2]=0;d=en(1024)|0;c[b>>2]=d;Nn(d|0,0,c[e>>2]<<3|0)|0;Nn(b+12|0,0,56)|0;if((a[9288]|0)==0){f=0;g=1}else{return}while(1){if((g|0)>(c[8376+(f<<2)>>2]|0)){b=f+1|0;a[9296+g|0]=b;h=b}else{a[9296+g|0]=f;h=f}b=g+1|0;if((b|0)<641){f=h;g=b}else{break}}a[9288]=1;return}function Um(a){a=a|0;var b=0,d=0,e=0,f=0,g=0,h=0,i=0,j=0;b=a+4|0;d=a|0;e=c[d>>2]|0;if((c[b>>2]|0)>0){f=0;g=e;while(1){fn(c[g+(f<<3)+4>>2]|0);h=f+1|0;i=c[d>>2]|0;if((h|0)<(c[b>>2]|0)){f=h;g=i}else{j=i;break}}}else{j=e}fn(j);j=a+68|0;ln(j);e=j|0;g=a+72|0;a=j|0;c[(c[a>>2]|0)+4>>2]=c[g>>2];c[c[g>>2]>>2]=c[a>>2];c[g>>2]=e;c[a>>2]=e;return}function Vm(a,b){a=a|0;b=b|0;var e=0,f=0,g=0,h=0,i=0,j=0,k=0,l=0,m=0,n=0;if((b|0)==0){e=0;return e|0}if((b|0)>640){e=jn(a+68|0,b)|0;return e|0}f=d[9296+b|0]|0;b=a+12+(f<<2)|0;g=c[b>>2]|0;if((g|0)!=0){c[b>>2]=c[g>>2];e=g;return e|0}g=a+4|0;h=c[g>>2]|0;i=a+8|0;j=a|0;if((h|0)==(c[i>>2]|0)){a=c[j>>2]|0;k=h+128|0;c[i>>2]=k;i=en(k<<3)|0;c[j>>2]=i;k=a;On(i|0,k|0,c[g>>2]<<3)|0;Nn((c[j>>2]|0)+(c[g>>2]<<3)|0,0,1024)|0;fn(k);l=c[g>>2]|0}else{l=h}h=c[j>>2]|0;j=en(16384)|0;k=h+(l<<3)+4|0;c[k>>2]=j;i=c[8376+(f<<2)>>2]|0;c[h+(l<<3)>>2]=i;l=(16384/(i|0)|0)-1|0;if((l|0)>0){h=0;f=j;while(1){a=h+1|0;c[f+(ba(h,i)|0)>>2]=f+(ba(a,i)|0);m=c[k>>2]|0;if((a|0)<(l|0)){h=a;f=m}else{n=m;break}}}else{n=j}c[n+(ba(l,i)|0)>>2]=0;c[b>>2]=c[c[k>>2]>>2];c[g>>2]=(c[g>>2]|0)+1;e=c[k>>2]|0;return e|0}function Wm(a,b,e){a=a|0;b=b|0;e=e|0;var f=0;if((e|0)==0){return}if((e|0)>640){kn(a+68|0,b);return}else{f=a+12+((d[9296+e|0]|0)<<2)|0;c[b>>2]=c[f>>2];c[f>>2]=b;return}}



function nk(a,b,d){a=a|0;b=b|0;d=d|0;var e=0,f=0,g=0,h=0,i=0,j=0,k=0,l=0,m=0,n=0,o=0,p=0,q=0,r=0,s=0,t=0,u=0,v=0,w=0,x=0,y=0,z=0,A=0,B=0,C=0,D=0,E=0,F=0,G=0,H=0,I=0,J=0,K=0,L=0,M=0;e=a;a=b;a:while(1){b=a;f=a-8|0;g=f;h=e;b:while(1){i=h;j=b-i|0;k=j>>3;switch(k|0){case 3:{l=6;break a;break};case 2:{l=4;break a;break};case 0:case 1:{l=66;break a;break};case 4:{l=14;break a;break};case 5:{l=15;break a;break};default:{}}if((j|0)<248){l=21;break a}m=(k|0)/2|0;n=h+(m<<3)|0;do{if((j|0)>7992){o=(k|0)/4|0;p=h+(o<<3)|0;q=h+(o+m<<3)|0;o=ok(h,p,n,q,d)|0;if(!(tb[c[d>>2]&63](f,q)|0)){r=o;break}s=q;t=c[s>>2]|0;u=c[s+4>>2]|0;v=c[g+4>>2]|0;c[s>>2]=c[g>>2];c[s+4>>2]=v;c[g>>2]=t;c[g+4>>2]=u;if(!(tb[c[d>>2]&63](q,n)|0)){r=o+1|0;break}q=n;u=c[q>>2]|0;t=c[q+4>>2]|0;v=c[s+4>>2]|0;c[q>>2]=c[s>>2];c[q+4>>2]=v;c[s>>2]=u;c[s+4>>2]=t;if(!(tb[c[d>>2]&63](n,p)|0)){r=o+2|0;break}t=p;s=c[t>>2]|0;u=c[t+4>>2]|0;v=c[q+4>>2]|0;c[t>>2]=c[q>>2];c[t+4>>2]=v;c[q>>2]=s;c[q+4>>2]=u;if(!(tb[c[d>>2]&63](p,h)|0)){r=o+3|0;break}p=h;u=c[p>>2]|0;q=c[p+4>>2]|0;s=c[t+4>>2]|0;c[p>>2]=c[t>>2];c[p+4>>2]=s;c[t>>2]=u;c[t+4>>2]=q;r=o+4|0}else{o=tb[c[d>>2]&63](n,h)|0;q=tb[c[d>>2]&63](f,n)|0;if(!o){if(!q){r=0;break}o=n;t=c[o>>2]|0;u=c[o+4>>2]|0;s=c[g+4>>2]|0;c[o>>2]=c[g>>2];c[o+4>>2]=s;c[g>>2]=t;c[g+4>>2]=u;if(!(tb[c[d>>2]&63](n,h)|0)){r=1;break}u=h;t=c[u>>2]|0;s=c[u+4>>2]|0;p=c[o+4>>2]|0;c[u>>2]=c[o>>2];c[u+4>>2]=p;c[o>>2]=t;c[o+4>>2]=s;r=2;break}s=h;o=c[s>>2]|0;t=c[s+4>>2]|0;if(q){q=c[g+4>>2]|0;c[s>>2]=c[g>>2];c[s+4>>2]=q;c[g>>2]=o;c[g+4>>2]=t;r=1;break}q=n;p=c[q+4>>2]|0;c[s>>2]=c[q>>2];c[s+4>>2]=p;c[q>>2]=o;c[q+4>>2]=t;if(!(tb[c[d>>2]&63](f,n)|0)){r=1;break}t=c[q>>2]|0;o=c[q+4>>2]|0;p=c[g+4>>2]|0;c[q>>2]=c[g>>2];c[q+4>>2]=p;c[g>>2]=t;c[g+4>>2]=o;r=2}}while(0);do{if(tb[c[d>>2]&63](h,n)|0){w=f;x=r}else{m=f;while(1){y=m-8|0;if((h|0)==(y|0)){break}if(tb[c[d>>2]&63](y,n)|0){l=49;break}else{m=y}}if((l|0)==49){l=0;m=h;k=c[m>>2]|0;j=c[m+4>>2]|0;o=y;t=c[o+4>>2]|0;c[m>>2]=c[o>>2];c[m+4>>2]=t;c[o>>2]=k;c[o+4>>2]=j;w=y;x=r+1|0;break}j=h+8|0;if(tb[c[d>>2]&63](h,f)|0){z=j}else{o=j;while(1){if((o|0)==(f|0)){l=66;break a}A=o+8|0;if(tb[c[d>>2]&63](h,o)|0){break}else{o=A}}j=o;k=c[j>>2]|0;t=c[j+4>>2]|0;m=c[g+4>>2]|0;c[j>>2]=c[g>>2];c[j+4>>2]=m;c[g>>2]=k;c[g+4>>2]=t;z=A}if((z|0)==(f|0)){l=66;break a}else{B=f;C=z}while(1){t=C;while(1){D=t+8|0;if(tb[c[d>>2]&63](h,t)|0){E=B;break}else{t=D}}do{E=E-8|0;}while(tb[c[d>>2]&63](h,E)|0);if(!(t>>>0<E>>>0)){h=t;continue b}k=t;m=c[k>>2]|0;j=c[k+4>>2]|0;p=E;q=c[p+4>>2]|0;c[k>>2]=c[p>>2];c[k+4>>2]=q;c[p>>2]=m;c[p+4>>2]=j;B=E;C=D}}}while(0);o=h+8|0;c:do{if(o>>>0<w>>>0){j=w;p=o;m=x;q=n;while(1){k=p;while(1){F=k+8|0;if(tb[c[d>>2]&63](k,q)|0){k=F}else{G=j;break}}do{G=G-8|0;}while(!(tb[c[d>>2]&63](G,q)|0));if(k>>>0>G>>>0){H=k;I=m;J=q;break c}t=k;s=c[t>>2]|0;u=c[t+4>>2]|0;v=G;K=c[v+4>>2]|0;c[t>>2]=c[v>>2];c[t+4>>2]=K;c[v>>2]=s;c[v+4>>2]=u;j=G;p=F;m=m+1|0;q=(q|0)==(k|0)?G:q}}else{H=o;I=x;J=n}}while(0);do{if((H|0)==(J|0)){L=I}else{if(!(tb[c[d>>2]&63](J,H)|0)){L=I;break}n=H;o=c[n>>2]|0;q=c[n+4>>2]|0;m=J;p=c[m+4>>2]|0;c[n>>2]=c[m>>2];c[n+4>>2]=p;c[m>>2]=o;c[m+4>>2]=q;L=I+1|0}}while(0);if((L|0)==0){M=qk(h,H,d)|0;q=H+8|0;if(qk(q,a,d)|0){l=61;break}if(M){h=q;continue}}q=H;if((q-i|0)>=(b-q|0)){l=65;break}nk(h,H,d);h=H+8|0}if((l|0)==61){l=0;if(M){l=66;break}else{e=h;a=H;continue}}else if((l|0)==65){l=0;nk(H+8|0,a,d);e=h;a=H;continue}}if((l|0)==4){if(!(tb[c[d>>2]&63](f,h)|0)){return}H=h;e=c[H>>2]|0;M=c[H+4>>2]|0;L=c[g+4>>2]|0;c[H>>2]=c[g>>2];c[H+4>>2]=L;c[g>>2]=e;c[g+4>>2]=M;return}else if((l|0)==6){M=h+8|0;e=tb[c[d>>2]&63](M,h)|0;L=tb[c[d>>2]&63](f,M)|0;if(!e){if(!L){return}e=M;H=c[e>>2]|0;I=c[e+4>>2]|0;J=c[g+4>>2]|0;c[e>>2]=c[g>>2];c[e+4>>2]=J;c[g>>2]=H;c[g+4>>2]=I;if(!(tb[c[d>>2]&63](M,h)|0)){return}I=h;H=c[I>>2]|0;J=c[I+4>>2]|0;x=c[e+4>>2]|0;c[I>>2]=c[e>>2];c[I+4>>2]=x;c[e>>2]=H;c[e+4>>2]=J;return}J=h;e=c[J>>2]|0;H=c[J+4>>2]|0;if(L){L=c[g+4>>2]|0;c[J>>2]=c[g>>2];c[J+4>>2]=L;c[g>>2]=e;c[g+4>>2]=H;return}L=M;x=c[L+4>>2]|0;c[J>>2]=c[L>>2];c[J+4>>2]=x;c[L>>2]=e;c[L+4>>2]=H;if(!(tb[c[d>>2]&63](f,M)|0)){return}M=c[L>>2]|0;H=c[L+4>>2]|0;e=c[g+4>>2]|0;c[L>>2]=c[g>>2];c[L+4>>2]=e;c[g>>2]=M;c[g+4>>2]=H;return}else if((l|0)==14){ok(h,h+8|0,h+16|0,f,d)|0;return}else if((l|0)==15){H=h+8|0;M=h+16|0;e=h+24|0;ok(h,H,M,e,d)|0;if(!(tb[c[d>>2]&63](f,e)|0)){return}f=e;L=c[f>>2]|0;x=c[f+4>>2]|0;J=c[g+4>>2]|0;c[f>>2]=c[g>>2];c[f+4>>2]=J;c[g>>2]=L;c[g+4>>2]=x;if(!(tb[c[d>>2]&63](e,M)|0)){return}e=M;x=c[e>>2]|0;g=c[e+4>>2]|0;L=c[f+4>>2]|0;c[e>>2]=c[f>>2];c[e+4>>2]=L;c[f>>2]=x;c[f+4>>2]=g;if(!(tb[c[d>>2]&63](M,H)|0)){return}M=H;g=c[M>>2]|0;f=c[M+4>>2]|0;x=c[e+4>>2]|0;c[M>>2]=c[e>>2];c[M+4>>2]=x;c[e>>2]=g;c[e+4>>2]=f;if(!(tb[c[d>>2]&63](H,h)|0)){return}H=h;f=c[H>>2]|0;e=c[H+4>>2]|0;g=c[M+4>>2]|0;c[H>>2]=c[M>>2];c[H+4>>2]=g;c[M>>2]=f;c[M+4>>2]=e;return}else if((l|0)==21){pk(h,a,d);return}else if((l|0)==66){return}}function ok(a,b,d,e,f){a=a|0;b=b|0;d=d|0;e=e|0;f=f|0;var g=0,h=0,i=0,j=0,k=0,l=0,m=0,n=0,o=0;g=tb[c[f>>2]&63](b,a)|0;h=tb[c[f>>2]&63](d,b)|0;do{if(g){i=a;j=c[i>>2]|0;k=c[i+4>>2]|0;if(h){l=d;m=c[l+4>>2]|0;c[i>>2]=c[l>>2];c[i+4>>2]=m;c[l>>2]=j;c[l+4>>2]=k;n=1;break}l=b;m=c[l+4>>2]|0;c[i>>2]=c[l>>2];c[i+4>>2]=m;c[l>>2]=j;c[l+4>>2]=k;if(!(tb[c[f>>2]&63](d,b)|0)){n=1;break}k=c[l>>2]|0;j=c[l+4>>2]|0;m=d;i=c[m+4>>2]|0;c[l>>2]=c[m>>2];c[l+4>>2]=i;c[m>>2]=k;c[m+4>>2]=j;n=2}else{if(!h){n=0;break}j=b;m=c[j>>2]|0;k=c[j+4>>2]|0;i=d;l=c[i+4>>2]|0;c[j>>2]=c[i>>2];c[j+4>>2]=l;c[i>>2]=m;c[i+4>>2]=k;if(!(tb[c[f>>2]&63](b,a)|0)){n=1;break}k=a;i=c[k>>2]|0;m=c[k+4>>2]|0;l=c[j+4>>2]|0;c[k>>2]=c[j>>2];c[k+4>>2]=l;c[j>>2]=i;c[j+4>>2]=m;n=2}}while(0);if(!(tb[c[f>>2]&63](e,d)|0)){o=n;return o|0}h=d;g=c[h>>2]|0;m=c[h+4>>2]|0;j=e;e=c[j+4>>2]|0;c[h>>2]=c[j>>2];c[h+4>>2]=e;c[j>>2]=g;c[j+4>>2]=m;if(!(tb[c[f>>2]&63](d,b)|0)){o=n+1|0;return o|0}d=b;m=c[d>>2]|0;j=c[d+4>>2]|0;g=c[h+4>>2]|0;c[d>>2]=c[h>>2];c[d+4>>2]=g;c[h>>2]=m;c[h+4>>2]=j;if(!(tb[c[f>>2]&63](b,a)|0)){o=n+2|0;return o|0}b=a;a=c[b>>2]|0;f=c[b+4>>2]|0;j=c[d+4>>2]|0;c[b>>2]=c[d>>2];c[b+4>>2]=j;c[d>>2]=a;c[d+4>>2]=f;o=n+3|0;return o|0}function pk(a,b,d){a=a|0;b=b|0;d=d|0;var e=0,f=0,g=0,h=0,j=0,k=0,l=0,m=0,n=0,o=0,p=0,q=0,r=0,s=0,t=0;e=i;i=i+8|0;f=e|0;g=f;h=a+16|0;j=a+8|0;k=tb[c[d>>2]&63](j,a)|0;l=tb[c[d>>2]&63](h,j)|0;do{if(k){m=a;n=c[m>>2]|0;o=c[m+4>>2]|0;if(l){p=h;q=c[p+4>>2]|0;c[m>>2]=c[p>>2];c[m+4>>2]=q;c[p>>2]=n;c[p+4>>2]=o;break}p=j;q=c[p+4>>2]|0;c[m>>2]=c[p>>2];c[m+4>>2]=q;c[p>>2]=n;c[p+4>>2]=o;if(!(tb[c[d>>2]&63](h,j)|0)){break}o=c[p>>2]|0;n=c[p+4>>2]|0;q=h;m=c[q+4>>2]|0;c[p>>2]=c[q>>2];c[p+4>>2]=m;c[q>>2]=o;c[q+4>>2]=n}else{if(!l){break}n=j;q=c[n>>2]|0;o=c[n+4>>2]|0;m=h;p=c[m+4>>2]|0;c[n>>2]=c[m>>2];c[n+4>>2]=p;c[m>>2]=q;c[m+4>>2]=o;if(!(tb[c[d>>2]&63](j,a)|0)){break}o=a;m=c[o>>2]|0;q=c[o+4>>2]|0;p=c[n+4>>2]|0;c[o>>2]=c[n>>2];c[o+4>>2]=p;c[n>>2]=m;c[n+4>>2]=q}}while(0);j=a+24|0;if((j|0)==(b|0)){i=e;return}else{r=h;s=j}while(1){if(tb[c[d>>2]&63](s,r)|0){j=s;h=c[j+4>>2]|0;c[f>>2]=c[j>>2];c[f+4>>2]=h;h=r;j=s;while(1){t=h;l=j;k=c[t+4>>2]|0;c[l>>2]=c[t>>2];c[l+4>>2]=k;if((h|0)==(a|0)){break}k=h-8|0;if(tb[c[d>>2]&63](g,k)|0){j=h;h=k}else{break}}h=c[f+4>>2]|0;c[t>>2]=c[f>>2];c[t+4>>2]=h}h=s+8|0;if((h|0)==(b|0)){break}else{r=s;s=h}}i=e;return}function qk(a,b,d){a=a|0;b=b|0;d=d|0;var e=0,f=0,g=0,h=0,j=0,k=0,l=0,m=0,n=0,o=0,p=0,q=0,r=0,s=0,t=0,u=0,v=0,w=0,x=0;e=i;i=i+8|0;f=e|0;g=f;switch(b-a>>3|0){case 4:{ok(a,a+8|0,a+16|0,b-8|0,d)|0;h=1;i=e;return h|0};case 5:{j=a+8|0;k=a+16|0;l=a+24|0;m=b-8|0;ok(a,j,k,l,d)|0;if(!(tb[c[d>>2]&63](m,l)|0)){h=1;i=e;return h|0}n=l;o=c[n>>2]|0;p=c[n+4>>2]|0;q=m;m=c[q+4>>2]|0;c[n>>2]=c[q>>2];c[n+4>>2]=m;c[q>>2]=o;c[q+4>>2]=p;if(!(tb[c[d>>2]&63](l,k)|0)){h=1;i=e;return h|0}l=k;p=c[l>>2]|0;q=c[l+4>>2]|0;o=c[n+4>>2]|0;c[l>>2]=c[n>>2];c[l+4>>2]=o;c[n>>2]=p;c[n+4>>2]=q;if(!(tb[c[d>>2]&63](k,j)|0)){h=1;i=e;return h|0}k=j;q=c[k>>2]|0;n=c[k+4>>2]|0;p=c[l+4>>2]|0;c[k>>2]=c[l>>2];c[k+4>>2]=p;c[l>>2]=q;c[l+4>>2]=n;if(!(tb[c[d>>2]&63](j,a)|0)){h=1;i=e;return h|0}j=a;n=c[j>>2]|0;l=c[j+4>>2]|0;q=c[k+4>>2]|0;c[j>>2]=c[k>>2];c[j+4>>2]=q;c[k>>2]=n;c[k+4>>2]=l;h=1;i=e;return h|0};case 2:{l=b-8|0;if(!(tb[c[d>>2]&63](l,a)|0)){h=1;i=e;return h|0}k=a;n=c[k>>2]|0;q=c[k+4>>2]|0;j=l;l=c[j+4>>2]|0;c[k>>2]=c[j>>2];c[k+4>>2]=l;c[j>>2]=n;c[j+4>>2]=q;h=1;i=e;return h|0};case 3:{q=a+8|0;j=b-8|0;n=tb[c[d>>2]&63](q,a)|0;l=tb[c[d>>2]&63](j,q)|0;if(!n){if(!l){h=1;i=e;return h|0}n=q;k=c[n>>2]|0;p=c[n+4>>2]|0;o=j;m=c[o+4>>2]|0;c[n>>2]=c[o>>2];c[n+4>>2]=m;c[o>>2]=k;c[o+4>>2]=p;if(!(tb[c[d>>2]&63](q,a)|0)){h=1;i=e;return h|0}p=a;o=c[p>>2]|0;k=c[p+4>>2]|0;m=c[n+4>>2]|0;c[p>>2]=c[n>>2];c[p+4>>2]=m;c[n>>2]=o;c[n+4>>2]=k;h=1;i=e;return h|0}k=a;n=c[k>>2]|0;o=c[k+4>>2]|0;if(l){l=j;m=c[l+4>>2]|0;c[k>>2]=c[l>>2];c[k+4>>2]=m;c[l>>2]=n;c[l+4>>2]=o;h=1;i=e;return h|0}l=q;m=c[l+4>>2]|0;c[k>>2]=c[l>>2];c[k+4>>2]=m;c[l>>2]=n;c[l+4>>2]=o;if(!(tb[c[d>>2]&63](j,q)|0)){h=1;i=e;return h|0}q=c[l>>2]|0;o=c[l+4>>2]|0;n=j;j=c[n+4>>2]|0;c[l>>2]=c[n>>2];c[l+4>>2]=j;c[n>>2]=q;c[n+4>>2]=o;h=1;i=e;return h|0};case 0:case 1:{h=1;i=e;return h|0};default:{o=a+16|0;n=a+8|0;q=tb[c[d>>2]&63](n,a)|0;j=tb[c[d>>2]&63](o,n)|0;do{if(q){l=a;m=c[l>>2]|0;k=c[l+4>>2]|0;if(j){p=o;r=c[p+4>>2]|0;c[l>>2]=c[p>>2];c[l+4>>2]=r;c[p>>2]=m;c[p+4>>2]=k;break}p=n;r=c[p+4>>2]|0;c[l>>2]=c[p>>2];c[l+4>>2]=r;c[p>>2]=m;c[p+4>>2]=k;if(!(tb[c[d>>2]&63](o,n)|0)){break}k=c[p>>2]|0;m=c[p+4>>2]|0;r=o;l=c[r+4>>2]|0;c[p>>2]=c[r>>2];c[p+4>>2]=l;c[r>>2]=k;c[r+4>>2]=m}else{if(!j){break}m=n;r=c[m>>2]|0;k=c[m+4>>2]|0;l=o;p=c[l+4>>2]|0;c[m>>2]=c[l>>2];c[m+4>>2]=p;c[l>>2]=r;c[l+4>>2]=k;if(!(tb[c[d>>2]&63](n,a)|0)){break}k=a;l=c[k>>2]|0;r=c[k+4>>2]|0;p=c[m+4>>2]|0;c[k>>2]=c[m>>2];c[k+4>>2]=p;c[m>>2]=l;c[m+4>>2]=r}}while(0);n=a+24|0;if((n|0)==(b|0)){h=1;i=e;return h|0}else{s=o;t=0;u=n}while(1){if(tb[c[d>>2]&63](u,s)|0){n=u;o=c[n+4>>2]|0;c[f>>2]=c[n>>2];c[f+4>>2]=o;o=s;n=u;while(1){v=o;j=n;q=c[v+4>>2]|0;c[j>>2]=c[v>>2];c[j+4>>2]=q;if((o|0)==(a|0)){break}q=o-8|0;if(tb[c[d>>2]&63](g,q)|0){n=o;o=q}else{break}}o=c[f+4>>2]|0;c[v>>2]=c[f>>2];c[v+4>>2]=o;o=t+1|0;if((o|0)==8){break}else{w=o}}else{w=t}o=u+8|0;if((o|0)==(b|0)){h=1;x=34;break}else{s=u;t=w;u=o}}if((x|0)==34){i=e;return h|0}h=(u+8|0)==(b|0);i=e;return h|0}}return 0}function rk(a){a=a|0;Jn(a);return}function sk(a,b){a=a|0;b=b|0;return}function tk(a,b){a=a|0;b=b|0;return}function uk(a,b,c){a=a|0;b=b|0;c=c|0;return}function vk(a,b,c){a=a|0;b=b|0;c=c|0;return}function wk(a,b,d){a=a|0;b=b|0;d=d|0;c[a>>2]=b;c[a+4>>2]=on(b,d<<4)|0;c[a+8>>2]=d;Nn(a+12|0,0,16)|0;return}function xk(a){a=a|0;var b=0,d=0;b=c[a+24>>2]|0;d=a|0;if((b|0)!=0){qn(c[d>>2]|0,b)}qn(c[d>>2]|0,c[a+4>>2]|0);return}function yk(b,d,e,f){b=b|0;d=d|0;e=e|0;f=f|0;var g=0,h=0,i=0;g=b+12|0;h=c[g>>2]|0;c[g>>2]=h+1;g=c[b+4>>2]|0;b=d;d=g+(h<<4)|0;i=c[b+4>>2]|0;c[d>>2]=c[b>>2];c[d+4>>2]=i;c[g+(h<<4)+8>>2]=e;a[g+(h<<4)+12|0]=f&1;return}function zk(b,d,e){b=b|0;d=+d;e=+e;var f=0.0,h=0,i=0,j=0,k=0.0,l=0.0,m=0.0,n=0,o=0.0,p=0.0,q=0.0,r=0.0,s=0.0,t=0.0,u=0,v=0.0,w=0.0,x=0.0,y=0.0,z=0,A=0,B=0,C=0,D=0,E=0,F=0,G=0,H=0,I=0,J=0,K=0,L=0,M=0,N=0,O=0,P=0,Q=0,R=0,S=0,T=0,U=0,V=0,W=0,X=0,Y=0,Z=0,_=0,$=0,aa=0,ca=0,da=0,ea=0,fa=0,ga=0,ha=0,ia=0,ja=0,ka=0,la=0,ma=0,na=0,oa=0,pa=0,qa=0,ra=0,sa=0,ta=0,ua=0,va=0,wa=0,xa=0,ya=0,za=0,Aa=0,Ba=0,Ca=0,Da=0,Ea=0,Fa=0,Ga=0,Ha=0,Ia=0,Ja=0,Ka=0,La=0,Ma=0,Na=0,Oa=0,Pa=0,Qa=0,Ra=0,Sa=0,Ta=0,Ua=0,Va=0,Wa=0,Xa=0,Ya=0,Za=0,_a=0,$a=0,ab=0,bb=0,cb=0,db=0,eb=0,fb=0,gb=0,hb=0,ib=0,jb=0,kb=0,lb=0,mb=0,nb=0,ob=0,pb=0,qb=0,rb=0,sb=0,tb=0,ub=0,vb=0,wb=0,xb=0,yb=0,zb=0,Ab=0,Bb=0,Cb=0,Db=0,Eb=0,Fb=0,Gb=0,Hb=0,Ib=0,Jb=0,Kb=0,Lb=0,Mb=0,Nb=0,Ob=0,Pb=0,Qb=0,Rb=0,Sb=0,Tb=0,Ub=0,Vb=0,Wb=0,Xb=0,Yb=0,Zb=0,_b=0,$b=0,ac=0,bc=0,cc=0,dc=0,ec=0,fc=0,gc=0,hc=0,ic=0,jc=0;f=1.0/d;h=b+12|0;i=c[h>>2]|0;if((i|0)>0){j=c[b+4>>2]|0;d=3.4028234663852886e+38;k=3.4028234663852886e+38;l=-3.4028234663852886e+38;m=-3.4028234663852886e+38;n=0;while(1){if((a[j+(n<<4)+12|0]|0)==0){o=m;p=l;q=k;r=d}else{s=+g[j+(n<<4)>>2];t=+g[j+(n<<4)+4>>2];o=m>s?m:s;p=l>t?l:t;q=k<s?k:s;r=d<t?d:t}u=n+1|0;if((u|0)<(i|0)){d=r;k=q;l=p;m=o;n=u}else{v=r;w=q;x=p;y=o;break}}}else{v=3.4028234663852886e+38;w=3.4028234663852886e+38;x=-3.4028234663852886e+38;y=-3.4028234663852886e+38}o=w-e;w=v-e;n=~~(f*(y+e-o))+1|0;i=b+16|0;c[i>>2]=n;j=~~(f*(x+e-w))+1|0;u=b+20|0;c[u>>2]=j;z=b|0;A=on(c[z>>2]|0,ba(j<<2,n)|0)|0;n=b+24|0;c[n>>2]=A;j=c[i>>2]|0;B=c[u>>2]|0;C=ba(B,j)|0;a:do{if((C|0)>0){D=0;E=A;while(1){c[E+(D<<2)>>2]=0;F=D+1|0;if((F|0)>=(C|0)){break a}D=F;E=c[n>>2]|0}}}while(0);C=c[z>>2]|0;z=ba(j<<2,B)|0;B=on(C,z<<4)|0;if((c[h>>2]|0)>0){j=b+4|0;b=z;A=0;E=B;D=0;while(1){F=c[j>>2]|0;G=F+(D<<4)|0;H=F+(D<<4)+4|0;e=f*(+g[G>>2]-o);F=G;x=+e;y=+(f*(+g[H>>2]-w));g[F>>2]=x;g[F+4>>2]=y;F=~~e;I=~~+g[H>>2];do{if((I|F|0)>-1){H=c[i>>2]|0;if((F|0)>=(H|0)){J=E;K=A;L=b;break}if((I|0)>=(c[u>>2]|0)){J=E;K=A;L=b;break}M=(ba(H,I)|0)+F|0;if((A|0)<(b|0)){N=E;O=b}else{if((A|0)>0){H=0;do{H=H+1|0;}while((H|0)<(A|0))}H=(b|0)>0?b<<1:1;N=pn(C,E,H<<4)|0;O=H}c[N+(A<<4)>>2]=F;c[N+(A<<4)+4>>2]=I;c[N+(A<<4)+8>>2]=M;c[N+(A<<4)+12>>2]=G;J=N;K=A+1|0;L=O}else{J=E;K=A;L=b}}while(0);G=D+1|0;if((G|0)<(c[h>>2]|0)){b=L;A=K;E=J;D=G}else{P=J;Q=0;R=K;S=L;break}}}else{P=B;Q=0;R=0;S=z}b:while(1){z=Q;do{if((z|0)==(R|0)){break b}T=c[P+(z<<4)>>2]|0;U=c[P+(z<<4)+4>>2]|0;V=c[P+(z<<4)+8>>2]|0;W=c[P+(z<<4)+12>>2]|0;z=z+1|0;X=(c[n>>2]|0)+(V<<2)|0;}while((c[X>>2]|0)!=0);c[X>>2]=W;if((T|0)>0){B=T-1|0;L=V-1|0;do{if((R|0)<(S|0)){Y=R;Z=P;_=z;$=S}else{if((z|0)<(R|0)){K=z;do{J=P+(K-z<<4)|0;D=P+(K<<4)|0;c[J>>2]=c[D>>2];c[J+4>>2]=c[D+4>>2];c[J+8>>2]=c[D+8>>2];c[J+12>>2]=c[D+12>>2];K=K+1|0;}while((K|0)<(R|0))}K=R-z|0;if((K|0)<(S|0)){Y=K;Z=P;_=0;$=S;break}M=(S|0)>0?S<<1:1;Y=K;Z=pn(C,P,M<<4)|0;_=0;$=M}}while(0);c[Z+(Y<<4)>>2]=B;c[Z+(Y<<4)+4>>2]=U;c[Z+(Y<<4)+8>>2]=L;c[Z+(Y<<4)+12>>2]=W;aa=Z;ca=_;da=Y+1|0;ea=$}else{aa=P;ca=z;da=R;ea=S}if((U|0)>0){M=U-1|0;K=V-(c[i>>2]|0)|0;do{if((da|0)<(ea|0)){fa=da;ga=aa;ha=ca;ia=ea}else{if((ca|0)<(da|0)){D=ca;do{J=aa+(D-ca<<4)|0;E=aa+(D<<4)|0;c[J>>2]=c[E>>2];c[J+4>>2]=c[E+4>>2];c[J+8>>2]=c[E+8>>2];c[J+12>>2]=c[E+12>>2];D=D+1|0;}while((D|0)<(da|0))}D=da-ca|0;if((D|0)<(ea|0)){fa=D;ga=aa;ha=0;ia=ea;break}E=(ea|0)>0?ea<<1:1;fa=D;ga=pn(C,aa,E<<4)|0;ha=0;ia=E}}while(0);c[ga+(fa<<4)>>2]=T;c[ga+(fa<<4)+4>>2]=M;c[ga+(fa<<4)+8>>2]=K;c[ga+(fa<<4)+12>>2]=W;ja=ga;ka=ha;la=fa+1|0;ma=ia}else{ja=aa;ka=ca;la=da;ma=ea}if((T|0)<((c[i>>2]|0)-1|0)){z=T+1|0;L=V+1|0;do{if((la|0)<(ma|0)){na=la;oa=ja;pa=ka;qa=ma}else{if((ka|0)<(la|0)){B=ka;do{E=ja+(B-ka<<4)|0;D=ja+(B<<4)|0;c[E>>2]=c[D>>2];c[E+4>>2]=c[D+4>>2];c[E+8>>2]=c[D+8>>2];c[E+12>>2]=c[D+12>>2];B=B+1|0;}while((B|0)<(la|0))}B=la-ka|0;if((B|0)<(ma|0)){na=B;oa=ja;pa=0;qa=ma;break}D=(ma|0)>0?ma<<1:1;na=B;oa=pn(C,ja,D<<4)|0;pa=0;qa=D}}while(0);c[oa+(na<<4)>>2]=z;c[oa+(na<<4)+4>>2]=U;c[oa+(na<<4)+8>>2]=L;c[oa+(na<<4)+12>>2]=W;ra=oa;sa=pa;ta=na+1|0;ua=qa}else{ra=ja;sa=ka;ta=la;ua=ma}if((U|0)>=((c[u>>2]|0)-1|0)){P=ra;Q=sa;R=ta;S=ua;continue}K=U+1|0;M=(c[i>>2]|0)+V|0;do{if((ta|0)<(ua|0)){va=ta;wa=ra;xa=sa;ya=ua}else{if((sa|0)<(ta|0)){D=sa;do{B=ra+(D-sa<<4)|0;E=ra+(D<<4)|0;c[B>>2]=c[E>>2];c[B+4>>2]=c[E+4>>2];c[B+8>>2]=c[E+8>>2];c[B+12>>2]=c[E+12>>2];D=D+1|0;}while((D|0)<(ta|0))}D=ta-sa|0;if((D|0)<(ua|0)){va=D;wa=ra;xa=0;ya=ua;break}E=(ua|0)>0?ua<<1:1;va=D;wa=pn(C,ra,E<<4)|0;xa=0;ya=E}}while(0);c[wa+(va<<4)>>2]=T;c[wa+(va<<4)+4>>2]=K;c[wa+(va<<4)+8>>2]=M;c[wa+(va<<4)+12>>2]=W;P=wa;Q=xa;R=va+1|0;S=ya}ya=c[u>>2]|0;if((ya|0)>0){va=S;xa=R;Q=R;wa=P;W=0;T=c[i>>2]|0;ra=ya;while(1){if((T-1|0)>0){ua=va;sa=xa;ta=Q;V=wa;U=0;ma=T;while(1){la=(ba(ma,W)|0)+U|0;ka=c[n>>2]|0;ja=c[ka+(la<<2)>>2]|0;qa=la+1|0;na=c[ka+(qa<<2)>>2]|0;if((ja|0)==(na|0)){za=V;Aa=ta;Ba=sa;Ca=ua;Da=ma;Ea=U+1|0}else{do{if((sa|0)<(ua|0)){Fa=sa;Ga=V;Ha=ta;Ia=ua}else{if((ta|0)<(sa|0)){ka=ta;do{pa=V+(ka-ta<<4)|0;oa=V+(ka<<4)|0;c[pa>>2]=c[oa>>2];c[pa+4>>2]=c[oa+4>>2];c[pa+8>>2]=c[oa+8>>2];c[pa+12>>2]=c[oa+12>>2];ka=ka+1|0;}while((ka|0)<(sa|0))}ka=sa-ta|0;if((ka|0)<(ua|0)){Fa=ka;Ga=V;Ha=0;Ia=ua;break}oa=(ua|0)>0?ua<<1:1;Fa=ka;Ga=pn(C,V,oa<<4)|0;Ha=0;Ia=oa}}while(0);c[Ga+(Fa<<4)>>2]=U;c[Ga+(Fa<<4)+4>>2]=W;c[Ga+(Fa<<4)+8>>2]=la;c[Ga+(Fa<<4)+12>>2]=na;oa=Fa+1|0;ka=U+1|0;do{if((oa|0)<(Ia|0)){Ja=oa;Ka=Ga;La=Ha;Ma=Ia}else{if((Ha|0)<(oa|0)){pa=Ha;do{ea=Ga+(pa-Ha<<4)|0;da=Ga+(pa<<4)|0;c[ea>>2]=c[da>>2];c[ea+4>>2]=c[da+4>>2];c[ea+8>>2]=c[da+8>>2];c[ea+12>>2]=c[da+12>>2];pa=pa+1|0;}while((pa|0)<(oa|0))}pa=oa-Ha|0;if((pa|0)<(Ia|0)){Ja=pa;Ka=Ga;La=0;Ma=Ia;break}da=(Ia|0)>0?Ia<<1:1;Ja=pa;Ka=pn(C,Ga,da<<4)|0;La=0;Ma=da}}while(0);c[Ka+(Ja<<4)>>2]=ka;c[Ka+(Ja<<4)+4>>2]=W;c[Ka+(Ja<<4)+8>>2]=qa;c[Ka+(Ja<<4)+12>>2]=ja;za=Ka;Aa=La;Ba=Ja+1|0;Ca=Ma;Da=c[i>>2]|0;Ea=ka}if((Ea|0)<(Da-1|0)){ua=Ca;sa=Ba;ta=Aa;V=za;U=Ea;ma=Da}else{break}}Na=Ca;Oa=Ba;Pa=Aa;Qa=za;Ra=Da;Sa=c[u>>2]|0}else{Na=va;Oa=xa;Pa=Q;Qa=wa;Ra=T;Sa=ra}ma=W+1|0;if((ma|0)<(Sa|0)){va=Na;xa=Oa;Q=Pa;wa=Qa;W=ma;T=Ra;ra=Sa}else{Ta=Na;Ua=Oa;Va=Pa;Wa=Qa;Xa=Sa;break}}}else{Ta=S;Ua=R;Va=R;Wa=P;Xa=ya}if((Xa-1|0)>0){ya=Ta;P=Ua;R=Va;S=Wa;Sa=0;Qa=c[i>>2]|0;Pa=Xa;while(1){Xa=Sa+1|0;if((Qa|0)>0){Oa=ya;Na=P;ra=R;Ra=S;T=0;W=Qa;while(1){wa=(ba(W,Sa)|0)+T|0;Q=c[n>>2]|0;xa=c[Q+(wa<<2)>>2]|0;va=c[Q+(wa+W<<2)>>2]|0;if((xa|0)==(va|0)){Ya=Ra;Za=ra;_a=Na;$a=Oa;ab=W}else{do{if((Na|0)<(Oa|0)){bb=Na;cb=Ra;db=ra;eb=Oa}else{if((ra|0)<(Na|0)){Q=ra;do{Da=Ra+(Q-ra<<4)|0;za=Ra+(Q<<4)|0;c[Da>>2]=c[za>>2];c[Da+4>>2]=c[za+4>>2];c[Da+8>>2]=c[za+8>>2];c[Da+12>>2]=c[za+12>>2];Q=Q+1|0;}while((Q|0)<(Na|0))}Q=Na-ra|0;if((Q|0)<(Oa|0)){bb=Q;cb=Ra;db=0;eb=Oa;break}za=(Oa|0)>0?Oa<<1:1;bb=Q;cb=pn(C,Ra,za<<4)|0;db=0;eb=za}}while(0);c[cb+(bb<<4)>>2]=T;c[cb+(bb<<4)+4>>2]=Sa;c[cb+(bb<<4)+8>>2]=wa;c[cb+(bb<<4)+12>>2]=va;ka=bb+1|0;ja=(c[i>>2]|0)+wa|0;do{if((ka|0)<(eb|0)){fb=ka;gb=cb;hb=db;ib=eb}else{if((db|0)<(ka|0)){qa=db;do{za=cb+(qa-db<<4)|0;Q=cb+(qa<<4)|0;c[za>>2]=c[Q>>2];c[za+4>>2]=c[Q+4>>2];c[za+8>>2]=c[Q+8>>2];c[za+12>>2]=c[Q+12>>2];qa=qa+1|0;}while((qa|0)<(ka|0))}qa=ka-db|0;if((qa|0)<(eb|0)){fb=qa;gb=cb;hb=0;ib=eb;break}Q=(eb|0)>0?eb<<1:1;fb=qa;gb=pn(C,cb,Q<<4)|0;hb=0;ib=Q}}while(0);c[gb+(fb<<4)>>2]=T;c[gb+(fb<<4)+4>>2]=Xa;c[gb+(fb<<4)+8>>2]=ja;c[gb+(fb<<4)+12>>2]=xa;Ya=gb;Za=hb;_a=fb+1|0;$a=ib;ab=c[i>>2]|0}ka=T+1|0;if((ka|0)<(ab|0)){Oa=$a;Na=_a;ra=Za;Ra=Ya;T=ka;W=ab}else{break}}jb=$a;kb=_a;lb=Za;mb=Ya;nb=ab;ob=c[u>>2]|0}else{jb=ya;kb=P;lb=R;mb=S;nb=Qa;ob=Pa}if((Xa|0)<(ob-1|0)){ya=jb;P=kb;R=lb;S=mb;Sa=Xa;Qa=nb;Pa=ob}else{pb=jb;qb=kb;rb=lb;sb=mb;break}}}else{pb=Ta;qb=Ua;rb=Va;sb=Wa}if((rb|0)==(qb|0)){tb=sb;ub=tb;qn(C,ub);return}else{vb=pb;wb=qb;xb=rb;yb=sb}c:while(1){sb=c[n>>2]|0;rb=xb;while(1){zb=c[yb+(rb<<4)>>2]|0;Ab=c[yb+(rb<<4)+4>>2]|0;Bb=c[yb+(rb<<4)+8>>2]|0;Cb=c[yb+(rb<<4)+12>>2]|0;Db=rb+1|0;Eb=sb+(Bb<<2)|0;qb=c[Eb>>2]|0;if((qb|0)!=(Cb|0)){w=+(zb|0);f=+g[qb>>2]-w;o=+(Ab|0);e=+g[qb+4>>2]-o;y=+g[Cb>>2]-w;w=+g[Cb+4>>2]-o;if(f*f+e*e>y*y+w*w){break}}if((Db|0)==(wb|0)){tb=yb;Fb=141;break c}else{rb=Db}}c[Eb>>2]=Cb;if((zb|0)>0){rb=zb-1|0;sb=Bb-1|0;do{if((wb|0)<(vb|0)){Gb=wb;Hb=yb;Ib=Db;Jb=vb}else{if((Db|0)<(wb|0)){Xa=Db;do{qb=yb+(Xa-Db<<4)|0;pb=yb+(Xa<<4)|0;c[qb>>2]=c[pb>>2];c[qb+4>>2]=c[pb+4>>2];c[qb+8>>2]=c[pb+8>>2];c[qb+12>>2]=c[pb+12>>2];Xa=Xa+1|0;}while((Xa|0)<(wb|0))}Xa=wb-Db|0;if((Xa|0)<(vb|0)){Gb=Xa;Hb=yb;Ib=0;Jb=vb;break}xa=(vb|0)>0?vb<<1:1;Gb=Xa;Hb=pn(C,yb,xa<<4)|0;Ib=0;Jb=xa}}while(0);c[Hb+(Gb<<4)>>2]=rb;c[Hb+(Gb<<4)+4>>2]=Ab;c[Hb+(Gb<<4)+8>>2]=sb;c[Hb+(Gb<<4)+12>>2]=Cb;Kb=Hb;Lb=Ib;Mb=Gb+1|0;Nb=Jb}else{Kb=yb;Lb=Db;Mb=wb;Nb=vb}if((Ab|0)>0){xa=Ab-1|0;Xa=Bb-(c[i>>2]|0)|0;do{if((Mb|0)<(Nb|0)){Ob=Mb;Pb=Kb;Qb=Lb;Rb=Nb}else{if((Lb|0)<(Mb|0)){ja=Lb;do{pb=Kb+(ja-Lb<<4)|0;qb=Kb+(ja<<4)|0;c[pb>>2]=c[qb>>2];c[pb+4>>2]=c[qb+4>>2];c[pb+8>>2]=c[qb+8>>2];c[pb+12>>2]=c[qb+12>>2];ja=ja+1|0;}while((ja|0)<(Mb|0))}ja=Mb-Lb|0;if((ja|0)<(Nb|0)){Ob=ja;Pb=Kb;Qb=0;Rb=Nb;break}qb=(Nb|0)>0?Nb<<1:1;Ob=ja;Pb=pn(C,Kb,qb<<4)|0;Qb=0;Rb=qb}}while(0);c[Pb+(Ob<<4)>>2]=zb;c[Pb+(Ob<<4)+4>>2]=xa;c[Pb+(Ob<<4)+8>>2]=Xa;c[Pb+(Ob<<4)+12>>2]=Cb;Sb=Pb;Tb=Qb;Ub=Ob+1|0;Vb=Rb}else{Sb=Kb;Tb=Lb;Ub=Mb;Vb=Nb}if((zb|0)<((c[i>>2]|0)-1|0)){sb=zb+1|0;rb=Bb+1|0;do{if((Ub|0)<(Vb|0)){Wb=Ub;Xb=Sb;Yb=Tb;Zb=Vb}else{if((Tb|0)<(Ub|0)){qb=Tb;do{ja=Sb+(qb-Tb<<4)|0;pb=Sb+(qb<<4)|0;c[ja>>2]=c[pb>>2];c[ja+4>>2]=c[pb+4>>2];c[ja+8>>2]=c[pb+8>>2];c[ja+12>>2]=c[pb+12>>2];qb=qb+1|0;}while((qb|0)<(Ub|0))}qb=Ub-Tb|0;if((qb|0)<(Vb|0)){Wb=qb;Xb=Sb;Yb=0;Zb=Vb;break}pb=(Vb|0)>0?Vb<<1:1;Wb=qb;Xb=pn(C,Sb,pb<<4)|0;Yb=0;Zb=pb}}while(0);c[Xb+(Wb<<4)>>2]=sb;c[Xb+(Wb<<4)+4>>2]=Ab;c[Xb+(Wb<<4)+8>>2]=rb;c[Xb+(Wb<<4)+12>>2]=Cb;_b=Xb;$b=Yb;ac=Wb+1|0;bc=Zb}else{_b=Sb;$b=Tb;ac=Ub;bc=Vb}if((Ab|0)<((c[u>>2]|0)-1|0)){Xa=Ab+1|0;xa=(c[i>>2]|0)+Bb|0;do{if((ac|0)<(bc|0)){cc=ac;dc=_b;ec=$b;fc=bc}else{if(($b|0)<(ac|0)){pb=$b;do{qb=_b+(pb-$b<<4)|0;ja=_b+(pb<<4)|0;c[qb>>2]=c[ja>>2];c[qb+4>>2]=c[ja+4>>2];c[qb+8>>2]=c[ja+8>>2];c[qb+12>>2]=c[ja+12>>2];pb=pb+1|0;}while((pb|0)<(ac|0))}pb=ac-$b|0;if((pb|0)<(bc|0)){cc=pb;dc=_b;ec=0;fc=bc;break}ja=(bc|0)>0?bc<<1:1;cc=pb;dc=pn(C,_b,ja<<4)|0;ec=0;fc=ja}}while(0);c[dc+(cc<<4)>>2]=zb;c[dc+(cc<<4)+4>>2]=Xa;c[dc+(cc<<4)+8>>2]=xa;c[dc+(cc<<4)+12>>2]=Cb;gc=dc;hc=ec;ic=cc+1|0;jc=fc}else{gc=_b;hc=$b;ic=ac;jc=bc}if((hc|0)==(ic|0)){tb=gc;Fb=141;break}else{vb=jc;wb=ic;xb=hc;yb=gc}}if((Fb|0)==141){ub=tb;qn(C,ub);return}}function Ak(b,d){b=b|0;d=d|0;var e=0,f=0,g=0,h=0,i=0,j=0,k=0,l=0,m=0,n=0,o=0,p=0,q=0,r=0,s=0,t=0;e=b+20|0;f=c[e>>2]|0;if((f-1|0)<=0){return}g=b+16|0;h=b+24|0;b=d;i=0;j=c[g>>2]|0;k=f;while(1){if((j-1|0)>0){f=0;l=j;do{m=(ba(l,i)|0)+f|0;n=c[h>>2]|0;o=c[n+(m<<2)>>2]|0;p=m+1|0;q=c[n+(p<<2)>>2]|0;r=c[n+(m+l<<2)>>2]|0;m=c[n+(p+l<<2)>>2]|0;a:do{if((q|0)!=(r|0)){b:do{if(!((o|0)==(q|0)|(o|0)==(r|0))){do{if((a[o+12|0]|0)==0){if((a[q+12|0]|0)!=0){break}if((a[r+12|0]|0)==0){break b}}}while(0);wb[c[(c[b>>2]|0)+8>>2]&63](d,c[o+8>>2]|0,c[q+8>>2]|0,c[r+8>>2]|0)}}while(0);if((m|0)==(q|0)|(m|0)==(r|0)){break}do{if((a[q+12|0]|0)==0){if((a[m+12|0]|0)!=0){break}if((a[r+12|0]|0)==0){break a}}}while(0);wb[c[(c[b>>2]|0)+8>>2]&63](d,c[q+8>>2]|0,c[m+8>>2]|0,c[r+8>>2]|0)}}while(0);f=f+1|0;l=c[g>>2]|0;}while((f|0)<(l-1|0));s=l;t=c[e>>2]|0}else{s=j;t=k}f=i+1|0;if((f|0)<(t-1|0)){i=f;j=s;k=t}else{break}}return}function Bk(a){a=a|0;var b=0,d=0,e=0;Nn(a|0,0,16)|0;g[a+16>>2]=1.0;c[a+20>>2]=0;c[a+24>>2]=0;c[a+28>>2]=-1;g[a+32>>2]=0.0;g[a+36>>2]=0.0;b=a+40|0;d=8600;e=c[d+4>>2]|0;c[b>>2]=c[d>>2];c[b+4>>2]=e;e=a+48|0;b=8600;d=c[b+4>>2]|0;c[e>>2]=c[b>>2];c[e+4>>2]=d;Nn(a+56|0,0,16)|0;g[a+72>>2]=1.0;c[a+76>>2]=0;return}function Ck(a){a=a|0;var b=0,d=0,e=0,f=0.0,h=0.0,i=0,j=0,k=0,l=0,m=0,n=0,o=0,p=0,q=0,r=0,s=0,t=0.0,u=0.0,v=0.0,w=0.0,x=0.0,y=0.0,z=0.0,A=0.0,B=0.0,C=0.0,D=0.0,E=0.0,F=0.0,G=0,H=0,I=0.0;b=a+28|0;d=c[a>>2]|0;e=c[d+4>>2]|0;if((c[b>>2]|0)==(e|0)){return}f=+g[d+32>>2]*.75;h=f*+g[d+320>>2]*f;i=a+32|0;g[i>>2]=0.0;j=a+40|0;k=a+44|0;l=a+48|0;m=a+52|0;Nn(j|0,0,16)|0;n=c[a+4>>2]|0;o=c[a+8>>2]|0;p=(n|0)<(o|0);do{if(p){q=c[d+96>>2]|0;r=c[d+104>>2]|0;s=n;f=0.0;t=0.0;u=0.0;v=0.0;w=0.0;do{f=h+f;g[i>>2]=f;x=h*+g[q+(s<<3)+4>>2];t=h*+g[q+(s<<3)>>2]+t;g[j>>2]=t;u=x+u;g[k>>2]=u;x=h*+g[r+(s<<3)+4>>2];v=h*+g[r+(s<<3)>>2]+v;g[l>>2]=v;w=x+w;g[m>>2]=w;s=s+1|0;}while((s|0)<(o|0));if(f>0.0){x=1.0/f;y=x*t;g[j>>2]=y;z=x*u;g[k>>2]=z;x=1.0/f;A=x*v;g[l>>2]=A;B=x*w;g[m>>2]=B;C=y;D=z;E=A;F=B}else{C=t;D=u;E=v;F=w}s=a+36|0;g[s>>2]=0.0;r=a+56|0;g[r>>2]=0.0;if(!p){break}q=c[d+96>>2]|0;G=c[d+104>>2]|0;H=n;B=0.0;A=0.0;do{z=+g[q+(H<<3)>>2]-C;y=+g[q+(H<<3)+4>>2]-D;x=+g[G+(H<<3)>>2]-E;I=+g[G+(H<<3)+4>>2]-F;B=h*(z*z+y*y)+B;g[s>>2]=B;A=A+h*(z*I-y*x);g[r>>2]=A;H=H+1|0;}while((H|0)<(o|0));if(!(B>0.0)){break}g[r>>2]=1.0/B*A}else{g[a+36>>2]=0.0;g[a+56>>2]=0.0}}while(0);c[b>>2]=e;return}function Dk(a,b){a=a|0;b=b|0;Al(c[a>>2]|0,c[a+4>>2]|0,c[a+8>>2]|0,b);return}function Ek(a,b){a=a|0;b=b|0;Bl(c[a>>2]|0,c[a+4>>2]|0,c[a+8>>2]|0,b);return}function Fk(a,b){a=a|0;b=b|0;var d=0,e=0,f=0,g=0,h=0,i=0;d=a|0;e=c[d>>2]|0;if((c[(c[e+400>>2]|0)+102876>>2]&2|0)!=0){return}f=c[a+4>>2]|0;g=a+8|0;if((f|0)<(c[g>>2]|0)){h=f;i=e}else{return}while(1){Qk(i,h,b);e=h+1|0;if((e|0)>=(c[g>>2]|0)){break}h=e;i=c[d>>2]|0}return}function Gk(b,d,e){b=b|0;d=d|0;e=e|0;var f=0,h=0,i=0,j=0,k=0,l=0,m=0,n=0.0;f=b+52|0;h=f|0;c[b+56>>2]=h;c[f>>2]=h;c[b+60>>2]=256;h=b+64|0;f=h|0;c[b+68>>2]=f;c[h>>2]=f;f=b+72|0;c[b+76>>2]=f;c[f>>2]=f;c[b+136>>2]=0;c[b+140>>2]=0;c[b+148>>2]=0;c[b+152>>2]=0;f=e|0;Nn(b+80|0,0,32)|0;Nn(b+160|0,0,36)|0;c[b+196>>2]=f;c[b+200>>2]=0;c[b+204>>2]=0;c[b+208>>2]=0;c[b+212>>2]=f;c[b+216>>2]=0;c[b+220>>2]=0;c[b+224>>2]=0;c[b+228>>2]=f;c[b+232>>2]=0;c[b+236>>2]=0;c[b+240>>2]=0;c[b+244>>2]=f;c[b+248>>2]=0;c[b+252>>2]=0;c[b+256>>2]=0;c[b+260>>2]=f;c[b+264>>2]=0;c[b+268>>2]=0;c[b+272>>2]=0;c[b+276>>2]=f;f=b+280|0;h=b+316|0;a[h]=0;i=b+320|0;Nn(f|0,0,16)|0;g[i>>2]=1.0;j=b+324|0;g[j>>2]=1.0;g[b+328>>2]=1.0;k=b+332|0;c[k>>2]=0;g[b+336>>2]=.05000000074505806;g[b+340>>2]=1.0;g[b+344>>2]=.25;g[b+348>>2]=.25;g[b+352>>2]=.25;g[b+356>>2]=.20000000298023224;g[b+360>>2]=.20000000298023224;g[b+364>>2]=1.0;g[b+368>>2]=.5;g[b+372>>2]=.5;g[b+376>>2]=.20000000298023224;g[b+380>>2]=.20000000298023224;c[b+384>>2]=8;g[b+388>>2]=.5;l=b+392|0;a[l]=1;g[b+396>>2]=.01666666753590107;a[b|0]=0;c[b+4>>2]=0;c[b+8>>2]=0;a[b+12|0]=0;c[b+16>>2]=0;a[b+20|0]=0;a[b+21|0]=0;c[b+24>>2]=0;m=d|0;a[h]=a[m]|0;n=+g[d+4>>2];g[i>>2]=n;g[b+28>>2]=1.0/n;g[j>>2]=+g[d+8>>2];n=+g[d+12>>2]*2.0;g[b+32>>2]=n;g[b+40>>2]=n*n;g[b+36>>2]=1.0/n;c[k>>2]=c[d+16>>2];c[b+44>>2]=0;d=b+48|0;c[d>>2]=0;c[b+144>>2]=0;c[b+308>>2]=0;c[b+312>>2]=0;Nn(b+112|0,0,24)|0;On(h|0,m|0,84)|0;m=b+400|0;c[m>>2]=e;c[b+156>>2]=0;e=b+296|0;c[e>>2]=0;c[e+4>>2]=0;a[b+304|0]=0;e=a[l]|0;if(e<<24>>24==0){a[l]=e;return}Kk(b,256);b=Vm(c[m>>2]|0,c[d>>2]<<2)|0;Nn(b|0,0,c[d>>2]<<2|0)|0;c[f>>2]=b;a[l]=e;return}function Hk(a){a=a|0;var b=0,d=0,e=0,f=0;b=a+312|0;while(1){d=c[b>>2]|0;if((d|0)==0){break}Ik(a,d)}do{if((c[a+84>>2]|0)==0){b=a+80|0;d=c[b>>2]|0;if((d|0)==0){break}Wm(c[a+400>>2]|0,d,c[a+48>>2]<<2);c[b>>2]=0}}while(0);do{if((c[a+92>>2]|0)==0){b=a+88|0;d=c[b>>2]|0;if((d|0)==0){break}Wm(c[a+400>>2]|0,d,c[a+48>>2]<<2);c[b>>2]=0}}while(0);do{if((c[a+164>>2]|0)==0){b=a+160|0;d=c[b>>2]|0;if((d|0)==0){break}Wm(c[a+400>>2]|0,d,c[a+48>>2]<<2);c[b>>2]=0}}while(0);do{if((c[a+172>>2]|0)==0){b=a+168|0;d=c[b>>2]|0;if((d|0)==0){break}Wm(c[a+400>>2]|0,d,c[a+48>>2]<<2);c[b>>2]=0}}while(0);do{if((c[a+180>>2]|0)==0){b=a+176|0;d=c[b>>2]|0;if((d|0)==0){break}Wm(c[a+400>>2]|0,d,c[a+48>>2]<<2);c[b>>2]=0}}while(0);do{if((c[a+100>>2]|0)==0){b=a+96|0;d=c[b>>2]|0;if((d|0)==0){break}Wm(c[a+400>>2]|0,d,c[a+48>>2]<<3);c[b>>2]=0}}while(0);do{if((c[a+108>>2]|0)==0){b=a+104|0;d=c[b>>2]|0;if((d|0)==0){break}Wm(c[a+400>>2]|0,d,c[a+48>>2]<<3);c[b>>2]=0}}while(0);do{if((c[a+140>>2]|0)==0){b=a+136|0;d=c[b>>2]|0;if((d|0)==0){break}Wm(c[a+400>>2]|0,d|0,c[a+48>>2]<<2);c[b>>2]=0}}while(0);do{if((c[a+152>>2]|0)==0){b=a+148|0;d=c[b>>2]|0;if((d|0)==0){break}Wm(c[a+400>>2]|0,d,c[a+48>>2]<<2);c[b>>2]=0}}while(0);do{if((c[a+284>>2]|0)==0){b=a+280|0;d=c[b>>2]|0;if((d|0)==0){break}Wm(c[a+400>>2]|0,d,c[a+48>>2]<<2);c[b>>2]=0}}while(0);do{if((c[a+292>>2]|0)==0){b=a+288|0;d=c[b>>2]|0;if((d|0)==0){break}Wm(c[a+400>>2]|0,d,c[a+48>>2]<<2);c[b>>2]=0}}while(0);b=a+112|0;d=a+48|0;e=c[b>>2]|0;if((e|0)!=0){Wm(c[a+400>>2]|0,e,c[d>>2]<<3);c[b>>2]=0}b=a+116|0;e=c[b>>2]|0;if((e|0)!=0){Wm(c[a+400>>2]|0,e,c[d>>2]<<2);c[b>>2]=0}b=a+120|0;e=c[b>>2]|0;if((e|0)!=0){Wm(c[a+400>>2]|0,e,c[d>>2]<<2);c[b>>2]=0}b=a+124|0;e=c[b>>2]|0;if((e|0)!=0){Wm(c[a+400>>2]|0,e,c[d>>2]<<2);c[b>>2]=0}b=a+128|0;e=c[b>>2]|0;if((e|0)!=0){Wm(c[a+400>>2]|0,e,c[d>>2]<<3);c[b>>2]=0}b=a+132|0;e=c[b>>2]|0;if((e|0)!=0){Wm(c[a+400>>2]|0,e,c[d>>2]<<2);c[b>>2]=0}b=a+144|0;e=c[b>>2]|0;if((e|0)!=0){Wm(c[a+400>>2]|0,e,c[d>>2]<<2);c[b>>2]=0}b=a+264|0;d=c[b>>2]|0;if((d|0)!=0){e=a+272|0;Wm(c[a+276>>2]|0,d,(c[e>>2]|0)*60|0);c[b>>2]=0;c[e>>2]=0;c[a+268>>2]=0}e=a+248|0;b=c[e>>2]|0;if((b|0)!=0){d=a+256|0;Wm(c[a+260>>2]|0,b,(c[d>>2]|0)*20|0);c[e>>2]=0;c[d>>2]=0;c[a+252>>2]=0}d=a+232|0;e=c[d>>2]|0;if((e|0)!=0){b=a+240|0;Wm(c[a+244>>2]|0,e,(c[b>>2]|0)*28|0);c[d>>2]=0;c[b>>2]=0;c[a+236>>2]=0}b=a+216|0;d=c[b>>2]|0;if((d|0)!=0){e=a+224|0;Wm(c[a+228>>2]|0,d,(c[e>>2]|0)*20|0);c[b>>2]=0;c[e>>2]=0;c[a+220>>2]=0}e=a+200|0;b=c[e>>2]|0;if((b|0)!=0){d=a+208|0;Wm(c[a+212>>2]|0,b,c[d>>2]<<3);c[e>>2]=0;c[d>>2]=0;c[a+204>>2]=0}d=a+184|0;e=c[d>>2]|0;if((e|0)==0){f=a+52|0;em(f);return}b=a+192|0;Wm(c[a+196>>2]|0,e,c[b>>2]<<2);c[d>>2]=0;c[b>>2]=0;c[a+188>>2]=0;f=a+52|0;em(f);return}function Ik(b,d){b=b|0;d=d|0;var e=0,f=0,g=0,h=0,i=0,j=0,k=0,l=0,m=0,n=0,o=0;e=b+400|0;f=c[(c[e>>2]|0)+102992>>2]|0;if((f|0)!=0){kb[c[(c[f>>2]|0)+16>>2]&255](f,d)}f=d+12|0;g=c[f>>2]|0;h=g<<4;i=h&16;if((((h|-17)^16)&g|0)!=0){a[b+20|0]=1}g=b+16|0;h=c[g>>2]|0;if((i&~h|0)!=0){c[g>>2]=h|i}c[f>>2]=i;i=c[d+4>>2]|0;f=c[d+8>>2]|0;if((i|0)<(f|0)){h=b+144|0;g=i;do{c[(c[h>>2]|0)+(g<<2)>>2]=0;g=g+1|0;}while((g|0)<(f|0))}f=d+20|0;g=c[f>>2]|0;h=d+24|0;if((g|0)!=0){c[g+24>>2]=c[h>>2]}g=c[h>>2]|0;if((g|0)!=0){c[g+20>>2]=c[f>>2]}f=b+312|0;if((c[f>>2]|0)!=(d|0)){j=b+308|0;k=c[j>>2]|0;l=k-1|0;c[j>>2]=l;m=c[e>>2]|0;n=m|0;o=d;Wm(n,o,80);return}c[f>>2]=c[h>>2];j=b+308|0;k=c[j>>2]|0;l=k-1|0;c[j>>2]=l;m=c[e>>2]|0;n=m|0;o=d;Wm(n,o,80);return}function Jk(a){a=a|0;var b=0,d=0,e=0,f=0,g=0;b=a+136|0;d=c[b>>2]|0;if((d|0)!=0){e=d;c[b>>2]=e;return e|0}d=a+48|0;f=c[d>>2]|0;if((f|0)==0){Kk(a,256);g=c[d>>2]|0}else{g=f}f=Vm(c[a+400>>2]|0,g<<2)|0;Nn(f|0,0,c[d>>2]<<2|0)|0;e=f;c[b>>2]=e;return e|0}function Kk(a,b){a=a|0;b=b|0;var d=0,e=0,f=0,g=0,h=0,i=0,j=0,k=0,l=0,m=0,n=0,o=0,p=0,q=0,r=0,s=0,t=0,u=0,v=0,w=0,x=0,y=0,z=0,A=0,B=0,C=0,D=0,E=0,F=0,G=0,H=0;d=c[a+332>>2]|0;if((d|0)==0){e=b}else{e=(d|0)<(b|0)?d:b}b=a+92|0;d=c[b>>2]|0;if((d|0)==0){f=e}else{f=(e|0)>(d|0)?d:e}e=a+100|0;d=c[e>>2]|0;if((d|0)==0){g=f}else{g=(f|0)>(d|0)?d:f}f=a+108|0;d=c[f>>2]|0;if((d|0)==0){h=g}else{h=(g|0)>(d|0)?d:g}g=a+140|0;d=c[g>>2]|0;if((d|0)==0){i=h}else{i=(h|0)>(d|0)?d:h}h=a+152|0;d=c[h>>2]|0;if((d|0)==0){j=i}else{j=(i|0)>(d|0)?d:i}i=a+48|0;d=c[i>>2]|0;if((d|0)>=(j|0)){return}k=a+80|0;l=c[k>>2]|0;m=(l|0)==0;do{if((c[a+84>>2]|0)==0&(m^1)){n=a+400|0;o=Vm(c[n>>2]|0,j<<2)|0;p=o;if(m){q=p;break}r=l;s=d<<2;On(o|0,r|0,s)|0;Wm(c[n>>2]|0,r,s);q=p}else{q=l}}while(0);c[k>>2]=q;q=c[i>>2]|0;c[a+60>>2]=j-q;k=a+88|0;l=c[k>>2]|0;do{if((c[b>>2]|0)==0){d=a+400|0;m=Vm(c[d>>2]|0,j<<2)|0;p=m;if((l|0)==0){t=p;break}s=l;r=q<<2;On(m|0,s|0,r)|0;Wm(c[d>>2]|0,s,r);t=p}else{t=l}}while(0);c[k>>2]=t;t=(c[a+156>>2]|0)>0;k=c[i>>2]|0;l=a+160|0;q=c[l>>2]|0;b=(q|0)==0;do{if((c[a+164>>2]|0)==0&(b&t^1)){p=a+400|0;r=Vm(c[p>>2]|0,j<<2)|0;s=r;if(b){u=s;break}d=q;m=k<<2;On(r|0,d|0,m)|0;Wm(c[p>>2]|0,d,m);u=s}else{u=q}}while(0);c[l>>2]=u;u=c[i>>2]|0;l=a+168|0;q=c[l>>2]|0;k=(q|0)==0;do{if((c[a+172>>2]|0)==0&(k&t^1)){b=a+400|0;s=Vm(c[b>>2]|0,j<<2)|0;m=s;if(k){v=m;break}d=q;p=u<<2;On(s|0,d|0,p)|0;Wm(c[b>>2]|0,d,p);v=m}else{v=q}}while(0);c[l>>2]=v;v=c[i>>2]|0;l=a+176|0;q=c[l>>2]|0;u=(q|0)==0;do{if((c[a+180>>2]|0)==0&(u&t^1)){k=a+400|0;m=Vm(c[k>>2]|0,j<<2)|0;p=m;if(u){w=p;break}d=q;b=v<<2;On(m|0,d|0,b)|0;Wm(c[k>>2]|0,d,b);w=p}else{w=q}}while(0);c[l>>2]=w;w=c[i>>2]|0;l=a+96|0;q=c[l>>2]|0;do{if((c[e>>2]|0)==0){v=a+400|0;u=Vm(c[v>>2]|0,j<<3)|0;t=u;if((q|0)==0){x=t;break}p=q;b=w<<3;On(u|0,p|0,b)|0;Wm(c[v>>2]|0,p,b);x=t}else{x=q}}while(0);c[l>>2]=x;x=c[i>>2]|0;l=a+104|0;q=c[l>>2]|0;do{if((c[f>>2]|0)==0){w=a+400|0;e=j<<3;t=Vm(c[w>>2]|0,e)|0;b=t;if((q|0)==0){y=b;z=w;A=e;break}p=q;v=x<<3;On(t|0,p|0,v)|0;Wm(c[w>>2]|0,p,v);y=b;z=w;A=e}else{y=q;z=a+400|0;A=j<<3}}while(0);c[l>>2]=y;y=a+112|0;l=c[y>>2]|0;q=c[i>>2]|0;x=Vm(c[z>>2]|0,A)|0;if((l|0)!=0){f=l;l=q<<3;On(x|0,f|0,l)|0;Wm(c[z>>2]|0,f,l)}c[y>>2]=x;x=a+116|0;y=c[x>>2]|0;l=c[i>>2]|0;f=j<<2;q=Vm(c[z>>2]|0,f)|0;if((y|0)!=0){e=y;y=l<<2;On(q|0,e|0,y)|0;Wm(c[z>>2]|0,e,y)}c[x>>2]=q;q=a+120|0;x=c[q>>2]|0;if((x|0)==0){B=0}else{y=c[i>>2]|0;e=Vm(c[z>>2]|0,f)|0;l=x;x=y<<2;On(e|0,l|0,x)|0;Wm(c[z>>2]|0,l,x);B=e}c[q>>2]=B;B=a+124|0;q=c[B>>2]|0;e=c[i>>2]|0;x=Vm(c[z>>2]|0,f)|0;if((q|0)!=0){l=q;q=e<<2;On(x|0,l|0,q)|0;Wm(c[z>>2]|0,l,q)}c[B>>2]=x;x=a+128|0;B=c[x>>2]|0;if((B|0)==0){C=0}else{q=c[i>>2]|0;l=Vm(c[z>>2]|0,A)|0;A=B;B=q<<3;On(l|0,A|0,B)|0;Wm(c[z>>2]|0,A,B);C=l}c[x>>2]=C;C=a+132|0;x=c[C>>2]|0;if((x|0)==0){D=0}else{l=c[i>>2]|0;B=Vm(c[z>>2]|0,f)|0;A=x;x=l<<2;On(B|0,A|0,x)|0;Wm(c[z>>2]|0,A,x);D=B}c[C>>2]=D;D=c[i>>2]|0;C=a+136|0;B=c[C>>2]|0;x=(B|0)==0;do{if((c[g>>2]|0)==0&(x^1)){A=Vm(c[z>>2]|0,f)|0;l=A;if(x){E=l;break}q=B|0;e=D<<2;On(A|0,q|0,e)|0;Wm(c[z>>2]|0,q,e);E=l}else{E=B}}while(0);c[C>>2]=E;E=a+144|0;C=c[E>>2]|0;B=c[i>>2]|0;D=Vm(c[z>>2]|0,f)|0;if((C|0)!=0){x=C;C=B<<2;On(D|0,x|0,C)|0;Wm(c[z>>2]|0,x,C)}c[E>>2]=D;D=c[i>>2]|0;E=a+148|0;C=c[E>>2]|0;x=(C|0)==0;do{if((c[h>>2]|0)==0&(x^1)){B=Vm(c[z>>2]|0,f)|0;g=B;if(x){F=g;break}l=C;e=D<<2;On(B|0,l|0,e)|0;Wm(c[z>>2]|0,l,e);F=g}else{F=C}}while(0);c[E>>2]=F;F=c[i>>2]|0;E=a+280|0;C=c[E>>2]|0;D=(C|0)==0;do{if((c[a+284>>2]|0)==0&(D^1)){x=Vm(c[z>>2]|0,f)|0;h=x;if(D){G=h;break}g=C;e=F<<2;On(x|0,g|0,e)|0;Wm(c[z>>2]|0,g,e);G=h}else{G=C}}while(0);c[E>>2]=G;G=c[i>>2]|0;E=a+288|0;C=c[E>>2]|0;F=(C|0)==0;do{if((c[a+292>>2]|0)==0&(F^1)){D=Vm(c[z>>2]|0,f)|0;h=D;if(F){H=h;break}e=C;g=G<<2;On(D|0,e|0,g)|0;Wm(c[z>>2]|0,e,g);H=h}else{H=C}}while(0);c[E>>2]=H;c[i>>2]=j;return}function Lk(b,d){b=b|0;d=d|0;var e=0,f=0,h=0,i=0,j=0,k=0,l=0,m=0,n=0,o=0,p=0,q=0,r=0,s=0,t=0,u=0,v=0,w=0,x=0.0,y=0.0;e=b+400|0;if((c[(c[e>>2]|0)+102876>>2]&2|0)!=0){f=0;return f|0}h=b+44|0;i=c[h>>2]|0;j=b+48|0;k=c[j>>2]|0;if((i|0)<(k|0)){l=i;m=k}else{Kk(b,(i|0)==0?256:i<<1);l=c[h>>2]|0;m=c[j>>2]|0}do{if((l|0)<(m|0)){n=l;o=b+88|0}else{if((a[b+392|0]|0)==0){f=-1;return f|0}else{i=c[b+288>>2]|0;k=c[i+(l-1<<2)>>2]|0;p=+(c[(c[b+280>>2]|0)+(k<<2)>>2]|0)>0.0?k:c[i>>2]|0;i=b+88|0;Pk(b,p,c[(c[i>>2]|0)+(p<<2)>>2]|2);Mk(b);n=c[h>>2]|0;o=i;break}}}while(0);l=n+1|0;c[h>>2]=l;c[(c[o>>2]|0)+(n<<2)>>2]=0;o=c[b+160>>2]|0;if((o|0)!=0){c[o+(n<<2)>>2]=0}o=c[b+168>>2]|0;if((o|0)!=0){c[o+(n<<2)>>2]=0}o=c[b+176>>2]|0;if((o|0)!=0){c[o+(n<<2)>>2]=0}o=d+4|0;h=(c[b+96>>2]|0)+(n<<3)|0;m=c[o+4>>2]|0;c[h>>2]=c[o>>2];c[h+4>>2]=m;m=d+12|0;h=(c[b+104>>2]|0)+(n<<3)|0;o=c[m+4>>2]|0;c[h>>2]=c[m>>2];c[h+4>>2]=o;g[(c[b+116>>2]|0)+(n<<2)>>2]=0.0;o=(c[b+112>>2]|0)+(n<<3)|0;h=8600;m=c[h+4>>2]|0;c[o>>2]=c[h>>2];c[o+4>>2]=m;m=c[b+120>>2]|0;if((m|0)!=0){g[m+(n<<2)>>2]=0.0}m=c[b+132>>2]|0;if((m|0)!=0){g[m+(n<<2)>>2]=0.0}m=b+136|0;o=c[m>>2]|0;h=d+20|0;a:do{if((o|0)==0){do{if((a[h]|0)==0){if((a[d+21|0]|0)!=0){break}if((a[d+22|0]|0)!=0){break}if((a[d+23|0]|0)==0){break a}}}while(0);i=c[j>>2]|0;if((i|0)==0){Kk(b,256);q=c[j>>2]|0}else{q=i}i=Vm(c[e>>2]|0,q<<2)|0;Nn(i|0,0,c[j>>2]<<2|0)|0;r=i;s=26}else{r=o;s=26}}while(0);if((s|0)==26){c[m>>2]=r;m=a[d+21|0]|0;o=a[d+22|0]|0;q=a[d+23|0]|0;a[r+(n<<2)|0]=a[h]|0;a[r+(n<<2)+1|0]=m;a[r+(n<<2)+2|0]=o;a[r+(n<<2)+3|0]=q}q=b+148|0;r=c[q>>2]|0;o=d+28|0;do{if((r|0)==0){if((c[o>>2]|0)==0){break}m=c[j>>2]|0;if((m|0)==0){Kk(b,256);t=c[j>>2]|0}else{t=m}m=Vm(c[e>>2]|0,t<<2)|0;Nn(m|0,0,c[j>>2]<<2|0)|0;u=m;s=32}else{u=r;s=32}}while(0);if((s|0)==32){c[q>>2]=u;c[u+(n<<2)>>2]=c[o>>2]}o=c[b+80>>2]|0;if((o|0)!=0){c[o+(n<<2)>>2]=0}o=b+200|0;u=b+204|0;q=c[u>>2]|0;s=b+208|0;r=c[s>>2]|0;if((q|0)<(r|0)){v=q;w=c[o>>2]|0}else{j=(q|0)==0?256:q<<1;q=b+212|0;t=Vm(c[q>>2]|0,j<<3)|0;e=t;m=o|0;o=c[m>>2]|0;if((o|0)!=0){h=r<<3;On(t|0,o|0,h)|0;Wm(c[q>>2]|0,c[m>>2]|0,h)}c[s>>2]=j;c[m>>2]=e;v=c[u>>2]|0;w=e}c[u>>2]=v+1;x=+g[d+24>>2];u=x>0.0;if((c[b+280>>2]|0)!=0|u){if(u){y=x}else{u=c[b+300>>2]|0;y=+g[b+396>>2]*+(((u|0)<0?-u|0:0)-u|0)}Nk(b,n,y);c[(c[b+288>>2]|0)+(n<<2)>>2]=n}c[w+(v<<3)>>2]=n;v=c[d+32>>2]|0;c[(c[b+144>>2]|0)+(n<<2)>>2]=v;do{if((v|0)!=0){w=v+4|0;u=c[w>>2]|0;e=v+8|0;m=c[e>>2]|0;if((u|0)<(m|0)){Ok(b,u,m,n);c[e>>2]=l;break}else{c[w>>2]=n;c[e>>2]=l;break}}}while(0);Pk(b,n,c[d>>2]|0);f=n;return f|0}function Mk(d){d=d|0;var f=0,h=0,i=0,j=0,k=0,l=0,m=0,n=0,o=0,p=0,q=0,r=0,s=0,t=0,u=0,v=0,w=0,x=0,y=0,z=0,A=0,B=0,C=0,D=0,E=0,F=0,G=0,H=0,I=0,J=0,K=0,L=0,M=0,N=0,O=0,P=0,Q=0,R=0,S=0,T=0,U=0,V=0,W=0,X=0,Y=0,Z=0,_=0,$=0,aa=0,ba=0,ca=0,da=0,ea=0,fa=0,ga=0,ha=0,ia=0,ja=0,ka=0,la=0,ma=0,na=0,oa=0,pa=0,qa=0,ra=0,sa=0,ta=0,ua=0,va=0,wa=0,xa=0,ya=0;f=d+400|0;h=d+44|0;i=on((c[f>>2]|0)+76|0,c[h>>2]<<2)|0;j=i;if((c[h>>2]|0)>0){k=d+88|0;l=d+80|0;m=d+160|0;n=d+168|0;o=d+176|0;p=d+96|0;q=d+104|0;r=d+144|0;s=d+21|0;t=d+120|0;u=d+132|0;v=d+136|0;w=d+148|0;x=d+280|0;y=d+112|0;z=d+64|0;A=0;B=0;C=0;while(1){D=c[k>>2]|0;E=c[D+(A<<2)>>2]|0;if((E&2|0)==0){c[j+(A<<2)>>2]=C;do{if((A|0)!=(C|0)){F=c[l>>2]|0;if((F|0)==0){G=D}else{H=c[F+(A<<2)>>2]|0;if((H|0)!=0){c[H+8>>2]=C}c[F+(C<<2)>>2]=H;G=c[k>>2]|0}c[G+(C<<2)>>2]=c[G+(A<<2)>>2];H=c[m>>2]|0;if((H|0)!=0){c[H+(C<<2)>>2]=c[H+(A<<2)>>2]}H=c[n>>2]|0;if((H|0)!=0){c[H+(C<<2)>>2]=c[H+(A<<2)>>2]}H=c[o>>2]|0;if((H|0)!=0){c[H+(C<<2)>>2]=c[H+(A<<2)>>2]}H=c[p>>2]|0;F=H+(A<<3)|0;I=H+(C<<3)|0;H=c[F+4>>2]|0;c[I>>2]=c[F>>2];c[I+4>>2]=H;H=c[q>>2]|0;I=H+(A<<3)|0;F=H+(C<<3)|0;H=c[I+4>>2]|0;c[F>>2]=c[I>>2];c[F+4>>2]=H;H=c[r>>2]|0;c[H+(C<<2)>>2]=c[H+(A<<2)>>2];if((a[s]|0)!=0){H=c[y>>2]|0;F=H+(A<<3)|0;I=H+(C<<3)|0;H=c[F+4>>2]|0;c[I>>2]=c[F>>2];c[I+4>>2]=H}H=c[t>>2]|0;if((H|0)!=0){g[H+(C<<2)>>2]=+g[H+(A<<2)>>2]}H=c[u>>2]|0;if((H|0)!=0){g[H+(C<<2)>>2]=+g[H+(A<<2)>>2]}H=c[v>>2]|0;if((H|0)!=0){I=a[H+(A<<2)+1|0]|0;F=a[H+(A<<2)+2|0]|0;J=a[H+(A<<2)+3|0]|0;a[H+(C<<2)|0]=a[H+(A<<2)|0]|0;a[H+(C<<2)+1|0]=I;a[H+(C<<2)+2|0]=F;a[H+(C<<2)+3|0]=J}J=c[w>>2]|0;if((J|0)!=0){c[J+(C<<2)>>2]=c[J+(A<<2)>>2]}J=c[x>>2]|0;if((J|0)==0){break}c[J+(C<<2)>>2]=c[J+(A<<2)>>2]}}while(0);K=C+1|0;L=E|B}else{D=c[(c[f>>2]|0)+102992>>2]|0;if(!((E&512|0)==0|(D|0)==0)){ob[c[(c[D>>2]|0)+20>>2]&31](D,d,A)}D=c[l>>2]|0;do{if((D|0)!=0){J=D+(A<<2)|0;H=c[J>>2]|0;if((H|0)==0){break}c[H+8>>2]=-1;c[J>>2]=0;Sm(z,H|0)}}while(0);c[j+(A<<2)>>2]=-1;K=C;L=B}D=A+1|0;if((D|0)<(c[h>>2]|0)){A=D;B=L;C=K}else{M=L;N=K;break}}}else{M=0;N=0}K=d+204|0;L=c[K>>2]|0;C=d+200|0;B=c[C>>2]|0;if((L|0)>0){A=0;while(1){z=B+(A<<3)|0;c[z>>2]=c[j+(c[z>>2]<<2)>>2];z=A+1|0;l=c[K>>2]|0;if((z|0)<(l|0)){A=z}else{O=l;break}}}else{O=L}L=B+(O<<3)|0;O=B;while(1){if((O|0)==(L|0)){P=L;Q=B;break}if((c[O>>2]|0)<0){R=O;S=O;T=40;break}else{O=O+8|0}}if((T|0)==40){a:while(1){T=0;O=R;do{O=O+8|0;if((O|0)==(L|0)){break a}}while((c[O>>2]|0)<0);B=O;A=S;l=c[B+4>>2]|0;c[A>>2]=c[B>>2];c[A+4>>2]=l;R=O;S=S+8|0;T=40}P=S;Q=c[C>>2]|0}c[K>>2]=P-Q>>3;Q=d+220|0;P=c[Q>>2]|0;K=c[d+216>>2]|0;if((P|0)>0){C=0;do{S=K+(C*20|0)|0;b[S>>1]=c[j+(e[S>>1]<<2)>>2];S=K+(C*20|0)+2|0;b[S>>1]=c[j+(e[S>>1]<<2)>>2];C=C+1|0;}while((C|0)<(P|0))}c[Q>>2]=(P*20|0|0)/20|0;P=d+236|0;Q=c[P>>2]|0;C=d+232|0;K=c[C>>2]|0;if((Q|0)>0){S=0;while(1){R=K+(S*28|0)|0;c[R>>2]=c[j+(c[R>>2]<<2)>>2];R=S+1|0;L=c[P>>2]|0;if((R|0)<(L|0)){S=R}else{U=L;break}}}else{U=Q}Q=K+(U*28|0)|0;U=K;while(1){if((U|0)==(Q|0)){V=Q;W=K;break}if((c[U>>2]|0)<0){X=U;Y=U;T=52;break}else{U=U+28|0}}if((T|0)==52){b:while(1){T=0;U=X;do{U=U+28|0;if((U|0)==(Q|0)){break b}}while((c[U>>2]|0)<0);O=Y;K=U;c[O>>2]=c[K>>2];c[O+4>>2]=c[K+4>>2];c[O+8>>2]=c[K+8>>2];c[O+12>>2]=c[K+12>>2];c[O+16>>2]=c[K+16>>2];c[O+20>>2]=c[K+20>>2];c[O+24>>2]=c[K+24>>2];X=U;Y=Y+28|0;T=52}V=Y;W=c[C>>2]|0}c[P>>2]=(V-W|0)/28|0;W=d+252|0;V=c[W>>2]|0;P=d+248|0;C=c[P>>2]|0;if((V|0)>0){Y=0;while(1){X=C+(Y*20|0)|0;c[X>>2]=c[j+(c[X>>2]<<2)>>2];X=C+(Y*20|0)+4|0;c[X>>2]=c[j+(c[X>>2]<<2)>>2];X=Y+1|0;Q=c[W>>2]|0;if((X|0)<(Q|0)){Y=X}else{Z=Q;break}}}else{Z=V}V=C+(Z*20|0)|0;Z=C;while(1){if((Z|0)==(V|0)){_=V;$=C;break}if((c[Z>>2]|0)<0){T=62;break}Y=Z+20|0;if((c[Z+4>>2]|0)<0){aa=Y;T=64;break}else{Z=Y}}if((T|0)==62){aa=Z+20|0;T=64}if((T|0)==64){c:do{if((aa|0)==(V|0)){ba=Z}else{C=Z;Y=Z;Q=aa;while(1){X=Y;K=Q;while(1){if((c[K>>2]|0)>=0){if((c[X+24>>2]|0)>=0){break}}O=K+20|0;if((O|0)==(V|0)){ba=C;break c}else{X=K;K=O}}X=C;O=K;c[X>>2]=c[O>>2];c[X+4>>2]=c[O+4>>2];c[X+8>>2]=c[O+8>>2];c[X+12>>2]=c[O+12>>2];c[X+16>>2]=c[O+16>>2];O=C+20|0;X=K+20|0;if((X|0)==(V|0)){ba=O;break}else{C=O;Y=K;Q=X}}}}while(0);_=ba;$=c[P>>2]|0}c[W>>2]=(_-$|0)/20|0;$=d+268|0;_=c[$>>2]|0;W=d+264|0;P=c[W>>2]|0;if((_|0)>0){ba=0;while(1){V=P+(ba*60|0)|0;c[V>>2]=c[j+(c[V>>2]<<2)>>2];V=P+(ba*60|0)+4|0;c[V>>2]=c[j+(c[V>>2]<<2)>>2];V=P+(ba*60|0)+8|0;c[V>>2]=c[j+(c[V>>2]<<2)>>2];V=ba+1|0;aa=c[$>>2]|0;if((V|0)<(aa|0)){ba=V}else{ca=aa;break}}}else{ca=_}_=P+(ca*60|0)|0;ca=P;while(1){if((ca|0)==(_|0)){da=_;ea=P;break}if((c[ca>>2]|0)<0){T=78;break}if((c[ca+4>>2]|0)<0){T=78;break}if((c[ca+8>>2]|0)<0){T=78;break}else{ca=ca+60|0}}if((T|0)==78){T=ca+60|0;d:do{if((T|0)==(_|0)){fa=ca}else{P=ca;ba=ca;aa=T;while(1){V=ba;Z=aa;e:while(1){do{if((c[Z>>2]|0)>=0){if((c[V+64>>2]|0)<0){break}if((c[V+68>>2]|0)>=0){break e}}}while(0);Q=Z+60|0;if((Q|0)==(_|0)){fa=P;break d}else{V=Z;Z=Q}}On(P|0,Z|0,60)|0;V=P+60|0;K=Z+60|0;if((K|0)==(_|0)){fa=V;break}else{P=V;ba=Z;aa=K}}}}while(0);da=fa;ea=c[W>>2]|0}c[$>>2]=(da-ea|0)/60|0;ea=c[d+288>>2]|0;do{if((ea|0)!=0){da=c[h>>2]|0;if((da|0)>0){ga=0;ha=0;ia=da}else{break}while(1){da=c[j+(c[ea+(ha<<2)>>2]<<2)>>2]|0;if((da|0)==-1){ja=ga;ka=ia}else{c[ea+(ga<<2)>>2]=da;ja=ga+1|0;ka=c[h>>2]|0}da=ha+1|0;if((da|0)<(ka|0)){ga=ja;ha=da;ia=ka}else{break}}}}while(0);ka=d+312|0;ia=c[ka>>2]|0;if((ia|0)!=0){ha=d+16|0;ja=d+132|0;ga=d+48|0;ea=d+20|0;da=ia;do{ia=da+4|0;$=c[ia>>2]|0;W=da+8|0;fa=c[W>>2]|0;if(($|0)<(fa|0)){_=N;T=0;ca=0;aa=$;while(1){$=c[j+(aa<<2)>>2]|0;if(($|0)>-1){ba=$+1|0;la=ca;ma=(T|0)>(ba|0)?T:ba;na=(_|0)<($|0)?_:$}else{la=1;ma=T;na=_}$=aa+1|0;if(($|0)<(fa|0)){_=na;T=ma;ca=la;aa=$}else{oa=na;pa=ma;qa=la;break}}}else{oa=N;pa=0;qa=0}do{if((oa|0)<(pa|0)){c[ia>>2]=oa;c[W>>2]=pa;if(!qa){break}aa=da+12|0;ca=c[aa>>2]|0;if((ca&1|0)==0){break}T=ca|16;ca=c[ha>>2]|0;if((T&~ca|0)!=0){_=c[ja>>2]|0;if((_|0)==0){fa=c[ga>>2]|0;if((fa|0)==0){Kk(d,256);ra=c[ga>>2]|0}else{ra=fa}fa=Vm(c[f>>2]|0,ra<<2)|0;Nn(fa|0,0,c[ga>>2]<<2|0)|0;sa=fa;ta=c[ha>>2]|0}else{sa=_;ta=ca}c[ja>>2]=sa;c[ha>>2]=ta|T}c[aa>>2]=T}else{c[ia>>2]=0;c[W>>2]=0;T=da+12|0;aa=c[T>>2]|0;if((aa&4|0)!=0){break}ca=aa|8;if(((aa&-9^-9)&aa|0)!=0){a[ea]=1}_=c[ha>>2]|0;if((ca&~_|0)!=0){if((aa&1|0)==0){ua=_}else{aa=c[ja>>2]|0;if((aa|0)==0){fa=c[ga>>2]|0;if((fa|0)==0){Kk(d,256);va=c[ga>>2]|0}else{va=fa}fa=Vm(c[f>>2]|0,va<<2)|0;Nn(fa|0,0,c[ga>>2]<<2|0)|0;wa=fa;xa=c[ha>>2]|0}else{wa=aa;xa=_}c[ja>>2]=wa;ua=xa}c[ha>>2]=ua|ca}c[T>>2]=ca}}while(0);da=c[da+24>>2]|0;}while((da|0)!=0)}c[h>>2]=N;qn((c[f>>2]|0)+76|0,i);c[d+8>>2]=M;a[d+12|0]=0;M=c[ka>>2]|0;if((M|0)==0){return}else{ya=M}while(1){M=c[ya+24>>2]|0;if((c[ya+12>>2]&8|0)!=0){Ik(d,ya)}if((M|0)==0){break}else{ya=M}}return}function Nk(b,d,e){b=b|0;d=d|0;e=+e;var f=0,h=0,i=0,j=0,k=0,l=0,m=0,n=0,o=0,p=0,q=0,r=0,s=0,t=0;f=b+288|0;h=c[f>>2]|0;i=(h|0)==0;j=b+280|0;k=c[j>>2]|0;if((k|0)==0){l=b+48|0;m=c[l>>2]|0;if((m|0)==0){Kk(b,256);n=c[l>>2]|0}else{n=m}m=Vm(c[b+400>>2]|0,n<<2)|0;Nn(m|0,0,c[l>>2]<<2|0)|0;o=m;p=c[f>>2]|0}else{o=k;p=h}c[j>>2]=o;if((p|0)==0){o=b+48|0;h=c[o>>2]|0;if((h|0)==0){Kk(b,256);q=c[o>>2]|0}else{q=h}h=Vm(c[b+400>>2]|0,q<<2)|0;Nn(h|0,0,c[o>>2]<<2|0)|0;r=h}else{r=p}c[f>>2]=r;do{if(i){f=c[b+44>>2]|0;if((f|0)>0){s=0}else{break}do{c[r+(s<<2)>>2]=s;s=s+1|0;}while((s|0)<(f|0))}}while(0);s=~~(e/+g[b+396>>2]);if((s|0)>0){t=(c[b+300>>2]|0)+s|0}else{t=s}s=(c[j>>2]|0)+(d<<2)|0;if((t|0)==(c[s>>2]|0)){return}c[s>>2]=t;a[b+304|0]=1;return}function Ok(d,e,f,h){d=d|0;e=e|0;f=f|0;h=h|0;var i=0,j=0,k=0,l=0,m=0,n=0,o=0,p=0,q=0,r=0,s=0,t=0,u=0,v=0,w=0,x=0,y=0,z=0,A=0,B=0,C=0,D=0,E=0,F=0,G=0,H=0,I=0,J=0,K=0,L=0,M=0,N=0,O=0,P=0,Q=0,R=0,S=0,T=0,U=0,V=0,W=0,X=0,Y=0.0,Z=0,_=0,$=0,aa=0.0,ba=0,ca=0,da=0,ea=0,fa=0,ga=0,ha=0,ia=0,ja=0,ka=0,la=0,ma=0,na=0,oa=0,pa=0,qa=0,ra=0,sa=0,ta=0,ua=0,va=0,wa=0,xa=0,ya=0,za=0;if((e|0)==(f|0)|(f|0)==(h|0)){return}i=c[d+88>>2]|0;j=i+(e<<2)|0;k=i+(f<<2)|0;l=i+(h<<2)|0;m=j;a:do{if((e+1|0)==(f|0)){n=c[j>>2]|0;o=l-k|0;Pn(m|0,k|0,o|0)|0;c[i+((o>>2)+e<<2)>>2]=n}else{if((f+1|0)==(h|0)){n=i+(h-1<<2)|0;o=c[n>>2]|0;p=n-j|0;Pn(i+(h-(p>>2)<<2)|0,m|0,p|0)|0;c[j>>2]=o;break}o=k;p=o-j>>2;n=l;q=n-o>>2;if((p|0)==(q|0)){o=j;r=k;while(1){s=c[o>>2]|0;c[o>>2]=c[r>>2];c[r>>2]=s;s=o+4|0;if((s|0)==(k|0)){break a}else{o=s;r=r+4|0}}}else{t=p;u=q}while(1){r=(t|0)%(u|0)|0;if((r|0)==0){break}else{t=u;u=r}}if((u|0)==0){break}q=p-1|0;r=p+e|0;o=i+(u+e<<2)|0;while(1){s=o-4|0;v=c[s>>2]|0;w=o+(q<<2)|0;x=s;while(1){c[x>>2]=c[w>>2];y=n-w>>2;if((p|0)<(y|0)){z=w+(p<<2)|0}else{z=i+(r-y<<2)|0}if((z|0)==(s|0)){break}else{x=w;w=z}}c[w>>2]=v;if((s|0)==(j|0)){break}else{o=s}}}}while(0);j=c[d+160>>2]|0;b:do{if((j|0)!=0){z=j+(e<<2)|0;i=j+(f<<2)|0;u=j+(h<<2)|0;t=z;if((e+1|0)==(f|0)){k=c[z>>2]|0;l=u-i|0;Pn(t|0,i|0,l|0)|0;c[j+((l>>2)+e<<2)>>2]=k;break}if((f+1|0)==(h|0)){k=j+(h-1<<2)|0;l=c[k>>2]|0;m=k-z|0;Pn(j+(h-(m>>2)<<2)|0,t|0,m|0)|0;c[z>>2]=l;break}l=i;m=l-z>>2;t=u;u=t-l>>2;if((m|0)==(u|0)){l=z;k=i;while(1){o=c[l>>2]|0;c[l>>2]=c[k>>2];c[k>>2]=o;o=l+4|0;if((o|0)==(i|0)){break b}else{l=o;k=k+4|0}}}else{A=m;B=u}while(1){k=(A|0)%(B|0)|0;if((k|0)==0){break}else{A=B;B=k}}if((B|0)==0){break}u=m-1|0;k=m+e|0;l=j+(B+e<<2)|0;while(1){i=l-4|0;o=c[i>>2]|0;r=l+(u<<2)|0;p=i;while(1){c[p>>2]=c[r>>2];n=t-r>>2;if((m|0)<(n|0)){C=r+(m<<2)|0}else{C=j+(k-n<<2)|0}if((C|0)==(i|0)){break}else{p=r;r=C}}c[r>>2]=o;if((i|0)==(z|0)){break}else{l=i}}}}while(0);C=c[d+168>>2]|0;c:do{if((C|0)!=0){j=C+(e<<2)|0;B=C+(f<<2)|0;A=C+(h<<2)|0;l=j;if((e+1|0)==(f|0)){z=c[j>>2]|0;k=A-B|0;Pn(l|0,B|0,k|0)|0;c[C+((k>>2)+e<<2)>>2]=z;break}if((f+1|0)==(h|0)){z=C+(h-1<<2)|0;k=c[z>>2]|0;m=z-j|0;Pn(C+(h-(m>>2)<<2)|0,l|0,m|0)|0;c[j>>2]=k;break}k=B;m=k-j>>2;l=A;A=l-k>>2;if((m|0)==(A|0)){k=j;z=B;while(1){t=c[k>>2]|0;c[k>>2]=c[z>>2];c[z>>2]=t;t=k+4|0;if((t|0)==(B|0)){break c}else{k=t;z=z+4|0}}}else{D=m;E=A}while(1){z=(D|0)%(E|0)|0;if((z|0)==0){break}else{D=E;E=z}}if((E|0)==0){break}A=m-1|0;z=m+e|0;k=C+(E+e<<2)|0;while(1){B=k-4|0;t=c[B>>2]|0;u=k+(A<<2)|0;p=B;while(1){c[p>>2]=c[u>>2];s=l-u>>2;if((m|0)<(s|0)){F=u+(m<<2)|0}else{F=C+(z-s<<2)|0}if((F|0)==(B|0)){break}else{p=u;u=F}}c[u>>2]=t;if((B|0)==(j|0)){break}else{k=B}}}}while(0);F=c[d+176>>2]|0;d:do{if((F|0)!=0){C=F+(e<<2)|0;E=F+(f<<2)|0;D=F+(h<<2)|0;k=C;if((e+1|0)==(f|0)){j=c[C>>2]|0;z=D-E|0;Pn(k|0,E|0,z|0)|0;c[F+((z>>2)+e<<2)>>2]=j;break}if((f+1|0)==(h|0)){j=F+(h-1<<2)|0;z=c[j>>2]|0;m=j-C|0;Pn(F+(h-(m>>2)<<2)|0,k|0,m|0)|0;c[C>>2]=z;break}z=E;m=z-C>>2;k=D;D=k-z>>2;if((m|0)==(D|0)){z=C;j=E;while(1){l=c[z>>2]|0;c[z>>2]=c[j>>2];c[j>>2]=l;l=z+4|0;if((l|0)==(E|0)){break d}else{z=l;j=j+4|0}}}else{G=m;H=D}while(1){j=(G|0)%(H|0)|0;if((j|0)==0){break}else{G=H;H=j}}if((H|0)==0){break}D=m-1|0;j=m+e|0;z=F+(H+e<<2)|0;while(1){E=z-4|0;l=c[E>>2]|0;A=z+(D<<2)|0;p=E;while(1){c[p>>2]=c[A>>2];i=k-A>>2;if((m|0)<(i|0)){I=A+(m<<2)|0}else{I=F+(j-i<<2)|0}if((I|0)==(E|0)){break}else{p=A;A=I}}c[A>>2]=l;if((E|0)==(C|0)){break}else{z=E}}}}while(0);I=c[d+96>>2]|0;F=I+(e<<3)|0;H=I+(f<<3)|0;G=I+(h<<3)|0;z=F;e:do{if((e+1|0)==(f|0)){C=F;j=c[C>>2]|0;m=c[C+4>>2]|0;C=G-H|0;Pn(z|0,H|0,C|0)|0;k=I+((C>>3)+e<<3)|0;c[k>>2]=j;c[k+4>>2]=m}else{if((f+1|0)==(h|0)){m=I+(h-1<<3)|0;k=m;j=c[k>>2]|0;C=c[k+4>>2]|0;k=m-F|0;Pn(I+(h-(k>>3)<<3)|0,z|0,k|0)|0;k=F;c[k>>2]=j;c[k+4>>2]=C;break}C=H;k=C-F>>3;j=G;m=j-C>>3;if((k|0)==(m|0)){C=F;D=H;while(1){p=C;B=c[p>>2]|0;t=c[p+4>>2]|0;u=D;i=c[u+4>>2]|0;c[p>>2]=c[u>>2];c[p+4>>2]=i;c[u>>2]=B;c[u+4>>2]=t;t=C+8|0;if((t|0)==(H|0)){break e}else{C=t;D=D+8|0}}}else{J=k;K=m}while(1){D=(J|0)%(K|0)|0;if((D|0)==0){break}else{J=K;K=D}}if((K|0)==0){break}m=k-1|0;D=k+e|0;C=I+(K+e<<3)|0;while(1){t=C-8|0;u=t;B=c[u>>2]|0;i=c[u+4>>2]|0;u=C+(m<<3)|0;p=t;while(1){L=u;o=p;r=c[L+4>>2]|0;c[o>>2]=c[L>>2];c[o+4>>2]=r;r=j-u>>3;if((k|0)<(r|0)){M=u+(k<<3)|0}else{M=I+(D-r<<3)|0}if((M|0)==(t|0)){break}else{p=u;u=M}}c[L>>2]=B;c[L+4>>2]=i;if((t|0)==(F|0)){break}else{C=t}}}}while(0);F=c[d+104>>2]|0;L=F+(e<<3)|0;M=F+(f<<3)|0;I=F+(h<<3)|0;K=L;f:do{if((e+1|0)==(f|0)){J=L;H=c[J>>2]|0;G=c[J+4>>2]|0;J=I-M|0;Pn(K|0,M|0,J|0)|0;z=F+((J>>3)+e<<3)|0;c[z>>2]=H;c[z+4>>2]=G}else{if((f+1|0)==(h|0)){G=F+(h-1<<3)|0;z=G;H=c[z>>2]|0;J=c[z+4>>2]|0;z=G-L|0;Pn(F+(h-(z>>3)<<3)|0,K|0,z|0)|0;z=L;c[z>>2]=H;c[z+4>>2]=J;break}J=M;z=J-L>>3;H=I;G=H-J>>3;if((z|0)==(G|0)){J=L;C=M;while(1){D=J;k=c[D>>2]|0;j=c[D+4>>2]|0;m=C;u=c[m+4>>2]|0;c[D>>2]=c[m>>2];c[D+4>>2]=u;c[m>>2]=k;c[m+4>>2]=j;j=J+8|0;if((j|0)==(M|0)){break f}else{J=j;C=C+8|0}}}else{N=z;O=G}while(1){C=(N|0)%(O|0)|0;if((C|0)==0){break}else{N=O;O=C}}if((O|0)==0){break}G=z-1|0;C=z+e|0;J=F+(O+e<<3)|0;while(1){j=J-8|0;m=j;k=c[m>>2]|0;u=c[m+4>>2]|0;m=J+(G<<3)|0;D=j;while(1){P=m;p=D;E=c[P+4>>2]|0;c[p>>2]=c[P>>2];c[p+4>>2]=E;E=H-m>>3;if((z|0)<(E|0)){Q=m+(z<<3)|0}else{Q=F+(C-E<<3)|0}if((Q|0)==(j|0)){break}else{D=m;m=Q}}c[P>>2]=k;c[P+4>>2]=u;if((j|0)==(L|0)){break}else{J=j}}}}while(0);L=c[d+144>>2]|0;P=L+(e<<2)|0;Q=L+(f<<2)|0;F=L+(h<<2)|0;O=P;g:do{if((e+1|0)==(f|0)){N=c[P>>2]|0;M=F-Q|0;Pn(O|0,Q|0,M|0)|0;c[L+((M>>2)+e<<2)>>2]=N}else{if((f+1|0)==(h|0)){N=L+(h-1<<2)|0;M=c[N>>2]|0;I=N-P|0;Pn(L+(h-(I>>2)<<2)|0,O|0,I|0)|0;c[P>>2]=M;break}M=Q;I=M-P>>2;N=F;K=N-M>>2;if((I|0)==(K|0)){M=P;J=Q;while(1){C=c[M>>2]|0;c[M>>2]=c[J>>2];c[J>>2]=C;C=M+4|0;if((C|0)==(Q|0)){break g}else{M=C;J=J+4|0}}}else{R=I;S=K}while(1){J=(R|0)%(S|0)|0;if((J|0)==0){break}else{R=S;S=J}}if((S|0)==0){break}K=I-1|0;J=I+e|0;M=L+(S+e<<2)|0;while(1){C=M-4|0;z=c[C>>2]|0;H=M+(K<<2)|0;G=C;while(1){c[G>>2]=c[H>>2];m=N-H>>2;if((I|0)<(m|0)){T=H+(I<<2)|0}else{T=L+(J-m<<2)|0}if((T|0)==(C|0)){break}else{G=H;H=T}}c[H>>2]=z;if((C|0)==(P|0)){break}else{M=C}}}}while(0);h:do{if((a[d+21|0]|0)!=0){P=c[d+112>>2]|0;T=P+(e<<3)|0;L=P+(f<<3)|0;S=P+(h<<3)|0;R=T;if((e+1|0)==(f|0)){Q=T;F=c[Q>>2]|0;O=c[Q+4>>2]|0;Q=S-L|0;Pn(R|0,L|0,Q|0)|0;M=P+((Q>>3)+e<<3)|0;c[M>>2]=F;c[M+4>>2]=O;break}if((f+1|0)==(h|0)){O=P+(h-1<<3)|0;M=O;F=c[M>>2]|0;Q=c[M+4>>2]|0;M=O-T|0;Pn(P+(h-(M>>3)<<3)|0,R|0,M|0)|0;M=T;c[M>>2]=F;c[M+4>>2]=Q;break}Q=L;M=Q-T>>3;F=S;S=F-Q>>3;if((M|0)==(S|0)){Q=T;R=L;while(1){O=Q;J=c[O>>2]|0;I=c[O+4>>2]|0;N=R;K=c[N+4>>2]|0;c[O>>2]=c[N>>2];c[O+4>>2]=K;c[N>>2]=J;c[N+4>>2]=I;I=Q+8|0;if((I|0)==(L|0)){break h}else{Q=I;R=R+8|0}}}else{U=M;V=S}while(1){R=(U|0)%(V|0)|0;if((R|0)==0){break}else{U=V;V=R}}if((V|0)==0){break}S=M-1|0;R=M+e|0;Q=P+(V+e<<3)|0;while(1){L=Q-8|0;I=L;N=c[I>>2]|0;J=c[I+4>>2]|0;I=Q+(S<<3)|0;K=L;while(1){W=I;O=K;G=c[W+4>>2]|0;c[O>>2]=c[W>>2];c[O+4>>2]=G;G=F-I>>3;if((M|0)<(G|0)){X=I+(M<<3)|0}else{X=P+(R-G<<3)|0}if((X|0)==(L|0)){break}else{K=I;I=X}}c[W>>2]=N;c[W+4>>2]=J;if((L|0)==(T|0)){break}else{Q=L}}}}while(0);W=c[d+120>>2]|0;i:do{if((W|0)!=0){X=W+(e<<2)|0;V=W+(f<<2)|0;U=W+(h<<2)|0;Q=X;if((e+1|0)==(f|0)){Y=+g[X>>2];T=U-V|0;Pn(Q|0,V|0,T|0)|0;g[W+((T>>2)+e<<2)>>2]=Y;break}if((f+1|0)==(h|0)){T=W+(h-1<<2)|0;Y=+g[T>>2];R=T-X|0;Pn(W+(h-(R>>2)<<2)|0,Q|0,R|0)|0;g[X>>2]=Y;break}R=V;Q=R-X>>2;T=U;U=T-R>>2;if((Q|0)==(U|0)){R=X;P=V;while(1){Y=+g[R>>2];g[R>>2]=+g[P>>2];g[P>>2]=Y;M=R+4|0;if((M|0)==(V|0)){break i}else{R=M;P=P+4|0}}}else{Z=Q;_=U}while(1){P=(Z|0)%(_|0)|0;if((P|0)==0){break}else{Z=_;_=P}}if((_|0)==0){break}U=Q-1|0;P=Q+e|0;R=W+(_+e<<2)|0;while(1){V=R-4|0;Y=+g[V>>2];M=R+(U<<2)|0;F=V;while(1){g[F>>2]=+g[M>>2];S=T-M>>2;if((Q|0)<(S|0)){$=M+(Q<<2)|0}else{$=W+(P-S<<2)|0}if(($|0)==(V|0)){break}else{F=M;M=$}}g[M>>2]=Y;if((V|0)==(X|0)){break}else{R=V}}}}while(0);$=c[d+132>>2]|0;j:do{if(($|0)!=0){W=$+(e<<2)|0;_=$+(f<<2)|0;Z=$+(h<<2)|0;R=W;if((e+1|0)==(f|0)){aa=+g[W>>2];X=Z-_|0;Pn(R|0,_|0,X|0)|0;g[$+((X>>2)+e<<2)>>2]=aa;break}if((f+1|0)==(h|0)){X=$+(h-1<<2)|0;aa=+g[X>>2];P=X-W|0;Pn($+(h-(P>>2)<<2)|0,R|0,P|0)|0;g[W>>2]=aa;break}P=_;R=P-W>>2;X=Z;Z=X-P>>2;if((R|0)==(Z|0)){P=W;Q=_;while(1){aa=+g[P>>2];g[P>>2]=+g[Q>>2];g[Q>>2]=aa;T=P+4|0;if((T|0)==(_|0)){break j}else{P=T;Q=Q+4|0}}}else{ba=R;ca=Z}while(1){Q=(ba|0)%(ca|0)|0;if((Q|0)==0){break}else{ba=ca;ca=Q}}if((ca|0)==0){break}Z=R-1|0;Q=R+e|0;P=$+(ca+e<<2)|0;while(1){_=P-4|0;aa=+g[_>>2];T=P+(Z<<2)|0;U=_;while(1){g[U>>2]=+g[T>>2];F=X-T>>2;if((R|0)<(F|0)){da=T+(R<<2)|0}else{da=$+(Q-F<<2)|0}if((da|0)==(_|0)){break}else{U=T;T=da}}g[T>>2]=aa;if((_|0)==(W|0)){break}else{P=_}}}}while(0);da=c[d+136>>2]|0;if((da|0)!=0){El(da+(e<<2)|0,da+(f<<2)|0,da+(h<<2)|0)|0}da=c[d+148>>2]|0;k:do{if((da|0)!=0){$=da+(e<<2)|0;ca=da+(f<<2)|0;ba=da+(h<<2)|0;P=$;if((e+1|0)==(f|0)){W=c[$>>2]|0;Q=ba-ca|0;Pn(P|0,ca|0,Q|0)|0;c[da+((Q>>2)+e<<2)>>2]=W;break}if((f+1|0)==(h|0)){W=da+(h-1<<2)|0;Q=c[W>>2]|0;R=W-$|0;Pn(da+(h-(R>>2)<<2)|0,P|0,R|0)|0;c[$>>2]=Q;break}Q=ca;R=Q-$>>2;P=ba;ba=P-Q>>2;if((R|0)==(ba|0)){Q=$;W=ca;while(1){X=c[Q>>2]|0;c[Q>>2]=c[W>>2];c[W>>2]=X;X=Q+4|0;if((X|0)==(ca|0)){break k}else{Q=X;W=W+4|0}}}else{ea=R;fa=ba}while(1){W=(ea|0)%(fa|0)|0;if((W|0)==0){break}else{ea=fa;fa=W}}if((fa|0)==0){break}ba=R-1|0;W=R+e|0;Q=da+(fa+e<<2)|0;while(1){ca=Q-4|0;X=c[ca>>2]|0;Z=Q+(ba<<2)|0;U=ca;while(1){c[U>>2]=c[Z>>2];V=P-Z>>2;if((R|0)<(V|0)){ga=Z+(R<<2)|0}else{ga=da+(W-V<<2)|0}if((ga|0)==(ca|0)){break}else{U=Z;Z=ga}}c[Z>>2]=X;if((ca|0)==($|0)){break}else{Q=ca}}}}while(0);ga=d+80|0;da=c[ga>>2]|0;do{if((da|0)!=0){fa=da+(e<<2)|0;ea=da+(f<<2)|0;Q=da+(h<<2)|0;$=fa;l:do{if((e+1|0)==(f|0)){W=c[fa>>2]|0;R=Q-ea|0;Pn($|0,ea|0,R|0)|0;c[da+((R>>2)+e<<2)>>2]=W}else{if((f+1|0)==(h|0)){W=da+(h-1<<2)|0;R=c[W>>2]|0;P=W-fa|0;Pn(da+(h-(P>>2)<<2)|0,$|0,P|0)|0;c[fa>>2]=R;break}R=ea;P=R-fa>>2;W=Q;ba=W-R>>2;if((P|0)==(ba|0)){R=fa;U=ea;while(1){_=c[R>>2]|0;c[R>>2]=c[U>>2];c[U>>2]=_;_=R+4|0;if((_|0)==(ea|0)){break l}else{R=_;U=U+4|0}}}else{ha=P;ia=ba}while(1){U=(ha|0)%(ia|0)|0;if((U|0)==0){break}else{ha=ia;ia=U}}if((ia|0)==0){break}ba=P-1|0;U=P+e|0;R=da+(ia+e<<2)|0;while(1){ca=R-4|0;X=c[ca>>2]|0;Z=R+(ba<<2)|0;_=ca;while(1){c[_>>2]=c[Z>>2];T=W-Z>>2;if((P|0)<(T|0)){ja=Z+(P<<2)|0}else{ja=da+(U-T<<2)|0}if((ja|0)==(ca|0)){break}else{_=Z;Z=ja}}c[Z>>2]=X;if((ca|0)==(fa|0)){break}else{R=ca}}}}while(0);if((e|0)>=(h|0)){break}fa=c[ga>>2]|0;ea=h-f|0;Q=e-f|0;$=e;do{R=c[fa+($<<2)>>2]|0;if((R|0)!=0){U=R+8|0;R=c[U>>2]|0;do{if((R|0)<(e|0)){ka=R}else{if((R|0)<(f|0)){ka=ea+R|0;break}else{ka=R+((R|0)<(h|0)?Q:0)|0;break}}}while(0);c[U>>2]=ka}$=$+1|0;}while(($|0)<(h|0))}}while(0);ka=c[d+280>>2]|0;do{if((ka|0)!=0){ga=ka+(e<<2)|0;ja=ka+(f<<2)|0;da=ka+(h<<2)|0;ia=ga;m:do{if((e+1|0)==(f|0)){ha=c[ga>>2]|0;$=da-ja|0;Pn(ia|0,ja|0,$|0)|0;c[ka+(($>>2)+e<<2)>>2]=ha}else{if((f+1|0)==(h|0)){ha=ka+(h-1<<2)|0;$=c[ha>>2]|0;Q=ha-ga|0;Pn(ka+(h-(Q>>2)<<2)|0,ia|0,Q|0)|0;c[ga>>2]=$;break}$=ja;Q=$-ga>>2;ha=da;ea=ha-$>>2;if((Q|0)==(ea|0)){$=ga;fa=ja;while(1){R=c[$>>2]|0;c[$>>2]=c[fa>>2];c[fa>>2]=R;R=$+4|0;if((R|0)==(ja|0)){break m}else{$=R;fa=fa+4|0}}}else{la=Q;ma=ea}while(1){fa=(la|0)%(ma|0)|0;if((fa|0)==0){break}else{la=ma;ma=fa}}if((ma|0)==0){break}ea=Q-1|0;fa=Q+e|0;$=ka+(ma+e<<2)|0;while(1){U=$-4|0;R=c[U>>2]|0;P=$+(ea<<2)|0;W=U;while(1){c[W>>2]=c[P>>2];ba=ha-P>>2;if((Q|0)<(ba|0)){na=P+(Q<<2)|0}else{na=ka+(fa-ba<<2)|0}if((na|0)==(U|0)){break}else{W=P;P=na}}c[P>>2]=R;if((U|0)==(ga|0)){break}else{$=U}}}}while(0);ga=c[d+44>>2]|0;ja=c[d+288>>2]|0;if((ga|0)<=0){break}da=h-f|0;ia=e-f|0;$=0;do{fa=ja+($<<2)|0;Q=c[fa>>2]|0;do{if((Q|0)<(e|0)){oa=Q}else{if((Q|0)<(f|0)){oa=da+Q|0;break}else{oa=Q+((Q|0)<(h|0)?ia:0)|0;break}}}while(0);c[fa>>2]=oa;$=$+1|0;}while(($|0)<(ga|0))}}while(0);oa=d+204|0;if((c[oa>>2]|0)>0){na=c[d+200>>2]|0;ka=h-f|0;ma=e-f|0;la=0;do{ga=na+(la<<3)|0;$=c[ga>>2]|0;do{if(($|0)<(e|0)){pa=$}else{if(($|0)<(f|0)){pa=ka+$|0;break}else{pa=$+(($|0)<(h|0)?ma:0)|0;break}}}while(0);c[ga>>2]=pa;la=la+1|0;}while((la|0)<(c[oa>>2]|0))}oa=c[d+220>>2]|0;if((oa|0)>0){la=c[d+216>>2]|0;pa=h-f|0;ma=e-f|0;ka=0;do{na=la+(ka*20|0)|0;$=b[na>>1]|0;ia=$&65535;do{if((ia|0)<(e|0)){qa=$}else{if((ia|0)<(f|0)){qa=pa+ia&65535;break}if((ia|0)>=(h|0)){qa=$;break}qa=ma+ia&65535}}while(0);b[na>>1]=qa;ia=la+(ka*20|0)+2|0;$=b[ia>>1]|0;ga=$&65535;do{if((ga|0)<(e|0)){ra=$}else{if((ga|0)<(f|0)){ra=pa+ga&65535;break}if((ga|0)>=(h|0)){ra=$;break}ra=ma+ga&65535}}while(0);b[ia>>1]=ra;ka=ka+1|0;}while((ka|0)<(oa|0))}oa=d+236|0;if((c[oa>>2]|0)>0){ka=c[d+232>>2]|0;ra=h-f|0;ma=e-f|0;pa=0;do{la=ka+(pa*28|0)|0;qa=c[la>>2]|0;do{if((qa|0)<(e|0)){sa=qa}else{if((qa|0)<(f|0)){sa=ra+qa|0;break}else{sa=qa+((qa|0)<(h|0)?ma:0)|0;break}}}while(0);c[la>>2]=sa;pa=pa+1|0;}while((pa|0)<(c[oa>>2]|0))}oa=d+252|0;if((c[oa>>2]|0)>0){pa=c[d+248>>2]|0;sa=h-f|0;ma=e-f|0;ra=0;do{ka=pa+(ra*20|0)|0;qa=c[ka>>2]|0;do{if((qa|0)<(e|0)){ta=qa}else{if((qa|0)<(f|0)){ta=sa+qa|0;break}else{ta=qa+((qa|0)<(h|0)?ma:0)|0;break}}}while(0);c[ka>>2]=ta;qa=pa+(ra*20|0)+4|0;la=c[qa>>2]|0;do{if((la|0)<(e|0)){ua=la}else{if((la|0)<(f|0)){ua=sa+la|0;break}else{ua=la+((la|0)<(h|0)?ma:0)|0;break}}}while(0);c[qa>>2]=ua;ra=ra+1|0;}while((ra|0)<(c[oa>>2]|0))}oa=d+268|0;if((c[oa>>2]|0)>0){ra=c[d+264>>2]|0;ua=h-f|0;ma=e-f|0;sa=0;do{pa=ra+(sa*60|0)|0;ta=c[pa>>2]|0;do{if((ta|0)<(e|0)){va=ta}else{if((ta|0)<(f|0)){va=ua+ta|0;break}else{va=ta+((ta|0)<(h|0)?ma:0)|0;break}}}while(0);c[pa>>2]=va;ta=ra+(sa*60|0)+4|0;qa=c[ta>>2]|0;do{if((qa|0)<(e|0)){wa=qa}else{if((qa|0)<(f|0)){wa=ua+qa|0;break}else{wa=qa+((qa|0)<(h|0)?ma:0)|0;break}}}while(0);c[ta>>2]=wa;qa=ra+(sa*60|0)+8|0;pa=c[qa>>2]|0;do{if((pa|0)<(e|0)){xa=pa}else{if((pa|0)<(f|0)){xa=ua+pa|0;break}else{xa=pa+((pa|0)<(h|0)?ma:0)|0;break}}}while(0);c[qa>>2]=xa;sa=sa+1|0;}while((sa|0)<(c[oa>>2]|0))}oa=c[d+312>>2]|0;if((oa|0)==0){return}d=h-f|0;sa=e-f|0;xa=oa;do{oa=xa+4|0;ma=c[oa>>2]|0;do{if((ma|0)<(e|0)){ya=ma}else{if((ma|0)<(f|0)){ya=d+ma|0;break}else{ya=ma+((ma|0)<(h|0)?sa:0)|0;break}}}while(0);c[oa>>2]=ya;ma=xa+8|0;qa=(c[ma>>2]|0)-1|0;do{if((qa|0)<(e|0)){za=qa}else{if((qa|0)<(f|0)){za=d+qa|0;break}else{za=qa+((qa|0)<(h|0)?sa:0)|0;break}}}while(0);c[ma>>2]=za+1;xa=c[xa+24>>2]|0;}while((xa|0)!=0);return}function Pk(b,d,e){b=b|0;d=d|0;e=e|0;var f=0,g=0,h=0,i=0,j=0,k=0,l=0,m=0,n=0;f=(c[b+88>>2]|0)+(d<<2)|0;if((c[f>>2]&~e|0)!=0){a[b+12|0]=1}d=b+8|0;if((~c[d>>2]&e|0)==0){c[f>>2]=e;return}if((e&128|0)!=0){g=b+128|0;h=c[g>>2]|0;if((h|0)==0){i=b+48|0;j=c[i>>2]|0;if((j|0)==0){Kk(b,256);k=c[i>>2]|0}else{k=j}j=Vm(c[b+400>>2]|0,k<<3)|0;Nn(j|0,0,c[i>>2]<<3|0)|0;l=j}else{l=h}c[g>>2]=l}if((e&256|0)!=0){l=b+136|0;g=c[l>>2]|0;if((g|0)==0){h=b+48|0;j=c[h>>2]|0;if((j|0)==0){Kk(b,256);m=c[h>>2]|0}else{m=j}j=Vm(c[b+400>>2]|0,m<<2)|0;Nn(j|0,0,c[h>>2]<<2|0)|0;n=j}else{n=g}c[l>>2]=n}c[d>>2]=c[d>>2]|e;c[f>>2]=e;return}function Qk(a,b,d){a=a|0;b=b|0;d=d|0;Pk(a,b,c[(c[a+88>>2]|0)+(b<<2)>>2]|(d?514:2));return}function Rk(b,d,e,f){b=b|0;d=d|0;e=e|0;f=f|0;var g=0,h=0,j=0,k=0,l=0,m=0;g=i;i=i+56|0;h=g|0;j=g+40|0;k=b+400|0;if((c[(c[k>>2]|0)+102876>>2]&2|0)!=0){l=0;i=g;return l|0}c[h>>2]=4288;c[h+4>>2]=b;c[h+8>>2]=d;b=h+12|0;m=e;c[b>>2]=c[m>>2];c[b+4>>2]=c[m+4>>2];c[b+8>>2]=c[m+8>>2];c[b+12>>2]=c[m+12>>2];a[h+28|0]=f&1;f=h+32|0;c[f>>2]=0;wb[c[(c[d>>2]|0)+28>>2]&63](d,j,e,0);Sj(c[k>>2]|0,h|0,j);l=c[f>>2]|0;i=g;return l|0}function Sk(b,d,e,f){b=b|0;d=d|0;e=e|0;f=f|0;var h=0,j=0,l=0,m=0.0,n=0.0,o=0,p=0,q=0,r=0,s=0,t=0,u=0,v=0,w=0,x=0,y=0,z=0,A=0,B=0,C=0,D=0,E=0,F=0,G=0,H=0,I=0,J=0,K=0,L=0,M=0,N=0,O=0,P=0,Q=0,S=0,T=0,U=0,V=0,W=0,X=0,Y=0,Z=0,_=0,$=0,aa=0,ba=0,ca=0,da=0,ea=0,fa=0,ga=0,ha=0,ia=0,ja=0,ka=0,la=0,ma=0,na=0,oa=0.0,pa=0.0,qa=0.0,ra=0.0,sa=0.0,ta=0.0,ua=0.0,va=0.0,wa=0.0,xa=0.0,ya=0.0,za=0.0,Aa=0.0,Ba=0.0,Ca=0.0,Da=0.0,Ea=0.0;h=i;i=i+88|0;j=h|0;l=h+40|0;m=+g[e+52>>2];if(m==0.0){n=+g[b+32>>2]*.75}else{n=m}o=mb[c[(c[d>>2]|0)+12>>2]&15](d)|0;if((o|0)<=0){i=h;return}p=l|0;q=l+4|0;r=l+8|0;s=l+28|0;t=d+4|0;u=d+8|0;v=d+12|0;w=l+12|0;x=d+20|0;y=l+20|0;z=d+28|0;A=l+28|0;B=d+36|0;C=l+36|0;D=d+44|0;E=l+44|0;F=d+45|0;G=l+45|0;H=l+20|0;I=l+12|0;J=l+24|0;K=l+16|0;L=j|0;M=j+4|0;N=j+12|0;O=j+20|0;P=j+21|0;Q=j+22|0;S=j+23|0;T=j+24|0;U=j+28|0;V=j+32|0;W=e|0;X=f+12|0;Y=f+8|0;Z=f|0;_=f+4|0;f=e+28|0;$=e+8|0;aa=e+12|0;ba=e+20|0;ca=e+24|0;da=e+32|0;ea=e+33|0;fa=e+34|0;ga=e+35|0;ha=e+64|0;ia=e+68|0;e=d;d=0;m=0.0;while(1){c[p>>2]=5976;c[q>>2]=1;g[r>>2]=.009999999776482582;Nn(s|0,0,18)|0;if((c[t>>2]|0)==1){c[q>>2]=1;g[r>>2]=+g[u>>2];ja=c[v>>2]|0;ka=c[v+4>>2]|0;c[w>>2]=ja;c[w+4>>2]=ka;la=c[x>>2]|0;ma=c[x+4>>2]|0;c[y>>2]=la;c[y+4>>2]=ma;na=c[z+4>>2]|0;c[A>>2]=c[z>>2];c[A+4>>2]=na;na=c[B+4>>2]|0;c[C>>2]=c[B>>2];c[C+4>>2]=na;a[E]=a[D]|0;a[G]=a[F]|0;oa=(c[k>>2]=la,+g[k>>2]);pa=(c[k>>2]=ja,+g[k>>2]);qa=(c[k>>2]=ma,+g[k>>2]);ra=oa;sa=qa;ta=pa;ua=(c[k>>2]=ka,+g[k>>2])}else{Ve(e,l,d);ra=+g[H>>2];sa=+g[J>>2];ta=+g[I>>2];ua=+g[K>>2]}pa=ra-ta;qa=sa-ua;oa=+R(pa*pa+qa*qa);a:do{if(m<oa){va=m;wa=ta;xa=ua;while(1){ya=va/oa;za=wa+pa*ya;Aa=qa*ya+xa;c[V>>2]=0;c[L>>2]=c[W>>2];ya=+g[X>>2];Ba=+g[Y>>2];Ca=+g[Z>>2]+(za*ya-Aa*Ba);Da=Aa*ya+za*Ba+ +g[_>>2];Ba=+Ca;za=+Da;g[M>>2]=Ba;g[M+4>>2]=za;za=+g[f>>2];Ba=+(+g[ba>>2]+(Da- +g[aa>>2])*(-0.0-za));Da=+(za*(Ca- +g[$>>2])+ +g[ca>>2]);g[N>>2]=Ba;g[N+4>>2]=Da;ka=a[ea]|0;ma=a[fa]|0;ja=a[ga]|0;a[O]=a[da]|0;a[P]=ka;a[Q]=ma;a[S]=ja;g[T>>2]=+g[ha>>2];c[U>>2]=c[ia>>2];Lk(b,j)|0;Da=n+va;if(!(Da<oa)){Ea=Da;break a}va=Da;wa=+g[I>>2];xa=+g[K>>2]}}else{Ea=m}}while(0);ja=d+1|0;if((ja|0)>=(o|0)){break}d=ja;m=Ea-oa}i=h;return}function Tk(b,d,e,f){b=b|0;d=d|0;e=e|0;f=f|0;var h=0,j=0,k=0,l=0,m=0,n=0.0,o=0.0,p=0,q=0.0,r=0,s=0,t=0,u=0,v=0,w=0,x=0,y=0,z=0,A=0,B=0,C=0,D=0,E=0,F=0,G=0,H=0,I=0,J=0,K=0,L=0,M=0,N=0,O=0,Q=0,R=0,S=0,T=0,U=0,V=0.0,W=0.0,X=0.0,Y=0.0,Z=0.0,_=0.0,$=0.0,aa=0.0,ba=0,ca=0,da=0.0,ea=0.0,fa=0.0;h=i;i=i+80|0;j=h|0;k=h+40|0;l=h+56|0;m=h+72|0;n=+g[e+52>>2];if(n==0.0){o=+g[b+32>>2]*.75}else{o=n}g[k>>2]=0.0;g[k+4>>2]=0.0;g[k+8>>2]=0.0;g[k+12>>2]=1.0;wb[c[(c[d>>2]|0)+28>>2]&63](d,l,k,0);n=o*+P(+g[l+4>>2]/o);p=l+12|0;q=+g[p>>2];if(!(n<q)){i=h;return}r=l|0;s=l+8|0;l=m|0;t=m+4|0;u=d;v=j|0;w=j+4|0;x=j+12|0;y=j+20|0;z=j+21|0;A=j+22|0;B=j+23|0;C=j+24|0;D=j+28|0;E=j+32|0;F=e|0;G=f+12|0;H=f+8|0;I=f|0;J=f+4|0;f=e+28|0;K=e+8|0;L=e+12|0;M=e+20|0;N=e+24|0;O=e+32|0;Q=e+33|0;R=e+34|0;S=e+35|0;T=e+64|0;U=e+68|0;V=n;n=+g[s>>2];W=q;while(1){q=o*+P(+g[r>>2]/o);if(q<n){X=q;do{g[l>>2]=X;g[t>>2]=V;if(lb[c[(c[u>>2]|0)+16>>2]&31](d,k,m)|0){c[E>>2]=0;c[v>>2]=c[F>>2];q=+g[G>>2];Y=+g[l>>2];Z=+g[H>>2];_=+g[t>>2];$=+g[I>>2]+(q*Y-Z*_);aa=Y*Z+q*_+ +g[J>>2];_=+$;q=+aa;g[w>>2]=_;g[w+4>>2]=q;q=+g[f>>2];_=+(+g[M>>2]+(aa- +g[L>>2])*(-0.0-q));aa=+(q*($- +g[K>>2])+ +g[N>>2]);g[x>>2]=_;g[x+4>>2]=aa;e=a[Q]|0;ba=a[R]|0;ca=a[S]|0;a[y]=a[O]|0;a[z]=e;a[A]=ba;a[B]=ca;g[C>>2]=+g[T>>2];c[D>>2]=c[U>>2];Lk(b,j)|0}X=o+X;da=+g[s>>2];}while(X<da);ea=da;fa=+g[p>>2]}else{ea=n;fa=W}X=o+V;if(X<fa){V=X;n=ea;W=fa}else{break}}i=h;return}function Uk(b,d){b=b|0;d=d|0;var e=0,f=0,h=0,j=0,k=0,l=0,m=0,n=0,o=0,p=0.0,q=0,r=0,s=0,t=0,u=0,v=0,w=0,x=0,y=0,z=0,A=0,B=0,C=0,D=0,E=0,F=0,G=0,H=0,I=0,J=0,K=0,L=0,M=0,N=0,O=0,P=0,Q=0.0,R=0.0,S=0.0,V=0.0,W=0.0,X=0.0,Y=0.0,Z=0,_=0,$=0,aa=0,ba=0,ca=0;e=i;i=i+80|0;f=e|0;h=e+40|0;j=e+64|0;k=j;l=i;i=i+4|0;i=i+7&-8;m=b+400|0;if((c[(c[m>>2]|0)+102876>>2]&2|0)!=0){n=0;i=e;return n|0}o=j|0;p=+g[d+16>>2];q=d+8|0;r=c[q+4>>2]|0;c[o>>2]=c[q>>2];c[o+4>>2]=r;r=j+8|0;g[r>>2]=+U(p);o=k+12|0;g[o>>2]=+T(p);q=b+44|0;s=c[q>>2]|0;t=c[d+40>>2]|0;do{if((t|0)!=0){u=c[t+4>>2]|0;if((u|0)==1|(u|0)==3){Sk(b,t,d,k);break}else if((u|0)==2|(u|0)==0){Tk(b,t,d,k);break}else{break}}}while(0);t=c[d+44>>2]|0;if((t|0)!=0){u=c[d+48>>2]|0;c[h>>2]=4232;c[h+12>>2]=t;c[h+16>>2]=u;Tk(b,h|0,d,k)}h=d+56|0;if((c[h>>2]|0)>0){u=d+60|0;t=f|0;v=f+4|0;w=f+12|0;x=f+20|0;y=f+21|0;z=f+22|0;A=f+23|0;B=f+24|0;C=f+28|0;D=f+32|0;E=d|0;F=d+28|0;G=d+8|0;H=d+12|0;I=d+20|0;J=d+24|0;K=d+32|0;L=d+33|0;M=d+34|0;N=d+35|0;O=d+64|0;P=d+68|0;p=+g[o>>2];Q=+g[r>>2];R=+g[j>>2];S=+g[k+4>>2];k=0;do{r=(c[u>>2]|0)+(k<<3)|0;V=+g[r>>2];W=+g[r+4>>2];c[D>>2]=0;c[t>>2]=c[E>>2];X=R+(V*p-W*Q);Y=p*W+V*Q+S;V=+X;W=+Y;g[v>>2]=V;g[v+4>>2]=W;W=+g[F>>2];V=+(+g[I>>2]+(Y- +g[H>>2])*(-0.0-W));Y=+(W*(X- +g[G>>2])+ +g[J>>2]);g[w>>2]=V;g[w+4>>2]=Y;r=a[L]|0;o=a[M]|0;Z=a[N]|0;a[x]=a[K]|0;a[y]=r;a[z]=o;a[A]=Z;g[B>>2]=+g[O>>2];c[C>>2]=c[P>>2];Lk(b,f)|0;k=k+1|0;}while((k|0)<(c[h>>2]|0))}h=c[q>>2]|0;q=Vm(c[m>>2]|0,80)|0;if((q|0)==0){_=0}else{k=q;Bk(k);_=k}c[_>>2]=b;c[_+4>>2]=s;c[_+8>>2]=h;g[_+16>>2]=+g[d+36>>2];c[_+76>>2]=c[d+68>>2];k=_+60|0;q=j;c[k>>2]=c[q>>2];c[k+4>>2]=c[q+4>>2];c[k+8>>2]=c[q+8>>2];c[k+12>>2]=c[q+12>>2];c[_+20>>2]=0;q=b+312|0;c[_+24>>2]=c[q>>2];k=c[q>>2]|0;if((k|0)!=0){c[k+20>>2]=_}c[q>>2]=_;q=b+308|0;c[q>>2]=(c[q>>2]|0)+1;if((s|0)<(h|0)){q=b+144|0;k=s;do{c[(c[q>>2]|0)+(k<<2)>>2]=_;k=k+1|0;}while((k|0)<(h|0))}k=c[d+4>>2]|0;q=_+12|0;j=c[q>>2]|0;f=((j^k)&1|0)==0?k:k|16;if((j&~f|0)!=0){a[b+20|0]=1}j=b+16|0;k=c[j>>2]|0;if((f&~k|0)!=0){if((f&1|0)==0){$=k}else{P=b+132|0;C=c[P>>2]|0;if((C|0)==0){O=b+48|0;B=c[O>>2]|0;if((B|0)==0){Kk(b,256);aa=c[O>>2]|0}else{aa=B}B=Vm(c[m>>2]|0,aa<<2)|0;Nn(B|0,0,c[O>>2]<<2|0)|0;ba=B;ca=c[j>>2]|0}else{ba=C;ca=k}c[P>>2]=ba;$=ca}c[j>>2]=$|f}c[q>>2]=f;c[l>>2]=4512;Vk(b,1);Wk(b,s,h,l);l=d+72|0;d=c[l>>2]|0;if((d|0)==0){n=_;i=e;return n|0}Xk(b,d,_);n=c[l>>2]|0;i=e;return n|0}function Vk(a,b){a=a|0;b=b|0;var d=0,e=0,f=0,h=0,j=0,k=0,l=0,m=0,n=0,o=0,p=0.0,q=0,r=0,s=0,t=0,u=0,v=0,w=0,x=0,y=0,z=0,A=0,B=0,C=0,D=0,E=0,F=0,G=0,H=0,I=0,J=0;d=i;i=i+32|0;e=d|0;f=d+8|0;h=d+16|0;j=a+204|0;k=c[j>>2]|0;l=a+200|0;m=c[l>>2]|0;n=m+(k<<3)|0;if((k|0)>0){o=c[a+96>>2]|0;p=+g[a+36>>2];q=m;do{r=c[q>>2]|0;c[q+4>>2]=(~~(p*+g[o+(r<<3)+4>>2]+2048.0)<<20)+~~(p*+g[o+(r<<3)>>2]*256.0+524288.0);q=q+8|0;}while(q>>>0<n>>>0);s=c[j>>2]|0}else{s=k}Sl(m,m+(s<<3)|0,f);f=a+400|0;s=c[f>>2]|0;m=h|0;c[m>>2]=0;c[h+4>>2]=0;k=h+8|0;c[k>>2]=0;n=h+12|0;c[n>>2]=s+76;q=a+8|0;do{if((c[q>>2]&32768|0)!=0){if((c[s+102952>>2]|0)==0){break}dl(h,c[a+216>>2]|0,c[a+220>>2]|0,c[a+88>>2]|0)}}while(0);s=a+216|0;o=c[l>>2]|0;l=c[j>>2]|0;j=o+(l<<3)|0;r=a+220|0;c[r>>2]=0;if((l|0)>0){l=o;t=o;while(1){o=t+4|0;u=c[o>>2]|0;v=u+256|0;w=t+8|0;x=w>>>0<j>>>0;if(x){y=t|0;z=w;do{if(v>>>0<(c[z+4>>2]|0)>>>0){break}bl(a,c[y>>2]|0,c[z>>2]|0,s);z=z+8|0;}while(z>>>0<j>>>0);A=c[o>>2]|0}else{A=u}z=A+1048320|0;y=l;while(1){if(!(y>>>0<j>>>0)){break}B=c[y+4>>2]|0;if(z>>>0>B>>>0){y=y+8|0}else{C=19;break}}a:do{if((C|0)==19){C=0;z=A+1048832|0;u=t|0;o=y;v=B;while(1){if(z>>>0<v>>>0){break a}bl(a,c[u>>2]|0,c[o>>2]|0,s);D=o+8|0;if(!(D>>>0<j>>>0)){break a}E=c[o+12>>2]|0;o=D;v=E}}}while(0);if(x){l=y;t=w}else{break}}}do{if((c[q>>2]&131072|0)!=0){t=c[(c[f>>2]|0)+102948>>2]|0;if((t|0)==0){break}c[e>>2]=a;c[e+4>>2]=t;cl(s,e)|0}}while(0);el(a,h);if(b){b=s|0;s=c[b>>2]|0;h=s+((c[r>>2]|0)*20|0)|0;a=s;while(1){if((a|0)==(h|0)){F=h;G=s;break}if((c[a+16>>2]&2|0)==0){a=a+20|0}else{H=a;I=a;C=32;break}}if((C|0)==32){b:while(1){C=0;a=H;while(1){J=a+20|0;if((J|0)==(h|0)){break b}if((c[a+36>>2]&2|0)==0){break}else{a=J}}a=I;w=J;c[a>>2]=c[w>>2];c[a+4>>2]=c[w+4>>2];c[a+8>>2]=c[w+8>>2];c[a+12>>2]=c[w+12>>2];c[a+16>>2]=c[w+16>>2];H=J;I=I+20|0;C=32}F=I;G=c[b>>2]|0}c[r>>2]=(F-G|0)/20|0}G=c[m>>2]|0;if((G|0)==0){i=d;return}qn(c[n>>2]|0,G);c[m>>2]=0;c[k>>2]=0;i=d;return}function Wk(a,b,d,f){a=a|0;b=b|0;d=d|0;f=f|0;var h=0,j=0,k=0,l=0,m=0,n=0,o=0,p=0,q=0,r=0,s=0,t=0,u=0,v=0,w=0,x=0,y=0,z=0,A=0,B=0,C=0,D=0,E=0,F=0,G=0,H=0,I=0,J=0,K=0,L=0,M=0,N=0,O=0,P=0,Q=0.0,S=0.0,T=0.0,U=0.0,V=0,W=0,X=0,Y=0,Z=0,_=0,$=0,aa=0,ba=0,ca=0,da=0,ea=0,fa=0,ga=0;h=i;i=i+64|0;j=h|0;k=h+8|0;l=h+16|0;m=h+48|0;n=(b|0)<(d|0);if(!n){i=h;return}o=c[a+88>>2]|0;p=b;q=0;do{q=c[o+(p<<2)>>2]|q;p=p+1|0;}while((p|0)<(d|0));if((q&1032|0)!=0){p=a+220|0;if((c[p>>2]|0)>0){o=a+216|0;r=a+88|0;s=a+144|0;t=f;u=f;v=a+252|0;w=a+256|0;x=a+248|0;y=a+96|0;z=a+260|0;A=0;while(1){B=c[o>>2]|0;C=e[B+(A*20|0)>>1]|0;D=e[B+(A*20|0)+2>>1]|0;E=c[r>>2]|0;F=c[E+(C<<2)>>2]|0;G=c[E+(D<<2)>>2]|0;E=c[s>>2]|0;H=c[E+(C<<2)>>2]|0;I=c[E+(D<<2)>>2]|0;do{if((D|0)<(d|0)&(((C|0)>=(d|0)|(C|0)<(b|0)|(D|0)<(b|0))^1)){E=G|F;if((E&2|0)!=0|(E&1032|0)==0){break}if(!(tb[c[(c[t>>2]|0)+8>>2]&63](f,C)|0)){if(!(tb[c[(c[t>>2]|0)+8>>2]&63](f,D)|0)){break}}if((F&28|0)==0){if((H|0)==0){break}if((c[H+12>>2]&2|0)==0){break}}if((G&28|0)==0){if((I|0)==0){break}if((c[I+12>>2]&2|0)==0){break}}if(!(lb[c[(c[u>>2]|0)+12>>2]&31](f,C,D)|0)){break}E=c[v>>2]|0;J=c[w>>2]|0;if((E|0)<(J|0)){K=E;L=c[x>>2]|0}else{M=(E|0)==0?256:E<<1;E=Vm(c[z>>2]|0,M*20|0)|0;N=E;O=c[x>>2]|0;if((O|0)!=0){P=J*20|0;On(E|0,O|0,P)|0;Wm(c[z>>2]|0,c[x>>2]|0,P)}c[w>>2]=M;c[x>>2]=N;K=c[v>>2]|0;L=N}c[v>>2]=K+1;c[L+(K*20|0)>>2]=C;c[L+(K*20|0)+4>>2]=D;c[L+(K*20|0)+8>>2]=c[B+(A*20|0)+16>>2];if((H|0)==0){Q=1.0}else{Q=+g[H+16>>2]}if((I|0)==0){S=1.0}else{S=+g[I+16>>2]}g[L+(K*20|0)+12>>2]=Q<S?Q:S;N=c[y>>2]|0;T=+g[N+(C<<3)>>2]- +g[N+(D<<3)>>2];U=+g[N+(C<<3)+4>>2]- +g[N+(D<<3)+4>>2];g[L+(K*20|0)+16>>2]=+R(T*T+U*U)}}while(0);D=A+1|0;if((D|0)<(c[p>>2]|0)){A=D}else{V=v;break}}}else{V=a+252|0}v=a+248|0;A=c[v>>2]|0;p=c[V>>2]|0;K=A+(p*20|0)|0;c[j>>2]=8;L=p*20|0;p=(L|0)/20|0;a:do{if((L|0)>2560){y=p;while(1){x=In(y*20|0,9080)|0;w=x;if((x|0)!=0){W=w;X=y;break a}if((y|0)>1){y=(y|0)/2|0}else{W=w;X=0;break}}}else{W=0;X=0}}while(0);$l(A,K,j,p,W,X);if((W|0)!=0){Jn(W)}W=c[v>>2]|0;X=c[V>>2]|0;p=W+(X*20|0)|0;b:do{if((X|0)==0){Y=W;Z=44}else if((X|0)==1){_=p;$=W}else{j=W;K=W+20|0;A=c[W>>2]|0;while(1){L=c[K>>2]|0;if((A|0)==(L|0)){if((c[j+4>>2]|0)==(c[j+24>>2]|0)){Y=j;Z=44;break b}}y=K+20|0;if((y|0)==(p|0)){_=p;$=W;break}else{j=K;K=y;A=L}}}}while(0);do{if((Z|0)==44){if((Y|0)==(p|0)){_=p;$=W;break}X=Y+20|0;A=Y;c:while(1){K=A|0;j=A+4|0;L=X;while(1){aa=L+20|0;if((aa|0)==(p|0)){break c}if((c[K>>2]|0)!=(c[aa>>2]|0)){break}if((c[j>>2]|0)==(c[L+24>>2]|0)){L=aa}else{break}}L=A+20|0;j=L;K=aa;c[j>>2]=c[K>>2];c[j+4>>2]=c[K+4>>2];c[j+8>>2]=c[K+8>>2];c[j+12>>2]=c[K+12>>2];c[j+16>>2]=c[K+16>>2];X=aa;A=L}_=A+20|0;$=c[v>>2]|0}}while(0);c[V>>2]=(_-$|0)/20|0}if((q&16|0)==0){i=h;return}wk(l,(c[a+400>>2]|0)+76|0,d-b|0);if(n){n=a+88|0;q=a+144|0;$=a+96|0;_=f;V=b;do{b=c[(c[n>>2]|0)+(V<<2)>>2]|0;do{if((b&2|0)==0){v=c[(c[q>>2]|0)+(V<<2)>>2]|0;if((b&28|0)==0){if((v|0)==0){break}if((c[v+12>>2]&2|0)==0){break}}v=c[$>>2]|0;yk(l,v+(V<<3)|0,V,tb[c[(c[_>>2]|0)+8>>2]&63](f,V)|0)}}while(0);V=V+1|0;}while((V|0)<(d|0))}S=+g[a+32>>2]*.75;zk(l,S*.5,S*2.0);c[m>>2]=4328;c[m+4>>2]=a;c[m+8>>2]=f;Ak(l,m|0);m=a+264|0;f=c[m>>2]|0;d=a+268|0;a=c[d>>2]|0;V=f+(a*60|0)|0;c[k>>2]=38;_=a*60|0;a=(_|0)/60|0;d:do{if((_|0)>7680){$=a;while(1){q=In($*60|0,9080)|0;n=q;if((q|0)!=0){ba=n;ca=$;break d}if(($|0)>1){$=($|0)/2|0}else{ba=n;ca=0;break}}}else{ba=0;ca=0}}while(0);Wl(f,V,k,a,ba,ca);if((ba|0)!=0){Jn(ba)}ba=c[m>>2]|0;ca=c[d>>2]|0;a=ba+(ca*60|0)|0;e:do{if((ca|0)==0){da=ba;Z=82}else if((ca|0)==1){ea=a;fa=ba}else{k=ba;V=ba+60|0;f=c[ba>>2]|0;while(1){_=c[V>>2]|0;do{if((f|0)==(_|0)){if((c[k+4>>2]|0)!=(c[k+64>>2]|0)){break}if((c[k+8>>2]|0)==(c[k+68>>2]|0)){da=k;Z=82;break e}}}while(0);$=V+60|0;if(($|0)==(a|0)){ea=a;fa=ba;break}else{k=V;V=$;f=_}}}}while(0);do{if((Z|0)==82){if((da|0)==(a|0)){ea=a;fa=ba;break}ca=da+60|0;f=da;f:while(1){V=f|0;k=f+4|0;$=f+8|0;n=ca;while(1){ga=n+60|0;if((ga|0)==(a|0)){break f}if((c[V>>2]|0)!=(c[ga>>2]|0)){break}if((c[k>>2]|0)!=(c[n+64>>2]|0)){break}if((c[$>>2]|0)==(c[n+68>>2]|0)){n=ga}else{break}}n=f+60|0;On(n|0,ga|0,60)|0;ca=ga;f=n}ea=f+60|0;fa=c[m>>2]|0}}while(0);c[d>>2]=(ea-fa|0)/60|0;xk(l);i=h;return}function Xk(b,d,e){b=b|0;d=d|0;e=e|0;var f=0,g=0,h=0,j=0,k=0,l=0,m=0,n=0,o=0,p=0,q=0,r=0,s=0,t=0,u=0,v=0;f=i;i=i+8|0;g=f|0;h=b+400|0;if((c[(c[h>>2]|0)+102876>>2]&2|0)!=0){i=f;return}j=e+4|0;k=e+8|0;Ok(b,c[j>>2]|0,c[k>>2]|0,c[b+44>>2]|0);l=d+4|0;m=d+8|0;Ok(b,c[l>>2]|0,c[m>>2]|0,c[j>>2]|0);n=c[j>>2]|0;c[g>>2]=4400;c[g+4>>2]=n;Vk(b,1);Wk(b,c[l>>2]|0,c[k>>2]|0,g|0);g=c[j>>2]|0;l=c[k>>2]|0;if((g|0)<(l|0)){n=b+144|0;o=g;do{c[(c[n>>2]|0)+(o<<2)>>2]=d;o=o+1|0;}while((o|0)<(l|0))}l=d+12|0;d=c[l>>2]|0;o=c[e+12>>2]|d;n=((o^d)&1|0)==0?o:o|16;if((d&~n|0)!=0){a[b+20|0]=1}d=b+16|0;o=c[d>>2]|0;if((n&~o|0)!=0){if((n&1|0)==0){p=o}else{g=b+132|0;q=c[g>>2]|0;if((q|0)==0){r=b+48|0;s=c[r>>2]|0;if((s|0)==0){Kk(b,256);t=c[r>>2]|0}else{t=s}s=Vm(c[h>>2]|0,t<<2)|0;Nn(s|0,0,c[r>>2]<<2|0)|0;u=s;v=c[d>>2]|0}else{u=q;v=o}c[g>>2]=u;p=v}c[d>>2]=p|n}c[l>>2]=n;c[m>>2]=c[k>>2];c[j>>2]=c[k>>2];Ik(b,e);i=f;return}function Yk(a){a=a|0;return}function Zk(a,b){a=a|0;b=b|0;var d=0,e=0,f=0;d=c[a>>2]|0;e=c[b>>2]|0;if((d|0)==(e|0)){f=(c[a+4>>2]|0)<(c[b+4>>2]|0);return f|0}else{f=(d-e|0)<0;return f|0}return 0}function _k(a,b){a=a|0;b=b|0;var d=0,e=0,f=0;d=c[a>>2]|0;e=c[b>>2]|0;if((d|0)!=(e|0)){f=(d-e|0)<0;return f|0}e=c[a+4>>2]|0;d=c[b+4>>2]|0;if((e|0)==(d|0)){f=(c[a+8>>2]|0)<(c[b+8>>2]|0);return f|0}else{f=(e-d|0)<0;return f|0}return 0}function $k(b){b=b|0;var d=0,f=0,h=0,i=0,j=0,k=0,l=0,m=0,n=0,o=0,p=0,q=0,r=0,s=0,t=0,u=0,v=0,w=0,x=0,y=0,z=0,A=0,B=0,C=0,D=0,E=0,F=0.0,G=0,H=0.0,I=0.0,J=0.0,K=0.0,L=0,M=0.0,N=0,O=0,P=0,Q=0,S=0,T=0.0;d=b+400|0;f=b+220|0;h=on((c[d>>2]|0)+76|0,(c[f>>2]|0)*20|0)|0;i=h;j=c[f>>2]|0;if((j|0)>0){k=b+216|0;l=b+144|0;m=0;n=0;o=j;while(1){j=c[k>>2]|0;p=j+(m*20|0)|0;q=c[l>>2]|0;r=c[q+((e[p>>1]|0)<<2)>>2]|0;do{if((r|0)==0){s=n;t=o}else{if((r|0)!=(c[q+((e[j+(m*20|0)+2>>1]|0)<<2)>>2]|0)){s=n;t=o;break}if((c[r+12>>2]&16|0)==0){s=n;t=o;break}u=i+(n*20|0)|0;v=p;c[u>>2]=c[v>>2];c[u+4>>2]=c[v+4>>2];c[u+8>>2]=c[v+8>>2];c[u+12>>2]=c[v+12>>2];c[u+16>>2]=c[v+16>>2];s=n+1|0;t=c[f>>2]|0}}while(0);p=m+1|0;if((p|0)<(t|0)){m=p;n=s;o=t}else{w=s;break}}}else{w=0}s=on((c[d>>2]|0)+76|0,c[b+308>>2]<<2)|0;t=s;o=c[b+312>>2]|0;if((o|0)==0){x=0}else{n=b+16|0;m=b+124|0;f=b+132|0;l=b+48|0;k=b+20|0;p=0;r=o;while(1){o=r+12|0;j=c[o>>2]|0;do{if((j&16|0)==0){y=p}else{q=p+1|0;c[t+(p<<2)>>2]=r;v=j&-17;if((((j|16)^-17)&j|0)!=0){a[k]=1}u=c[n>>2]|0;if((v&~u|0)!=0){if((j&1|0)==0){z=u}else{A=c[f>>2]|0;if((A|0)==0){B=c[l>>2]|0;if((B|0)==0){Kk(b,256);C=c[l>>2]|0}else{C=B}B=Vm(c[d>>2]|0,C<<2)|0;Nn(B|0,0,c[l>>2]<<2|0)|0;D=B;E=c[n>>2]|0}else{D=A;E=u}c[f>>2]=D;z=E}c[n>>2]=z|v}c[o>>2]=v;v=c[r+4>>2]|0;u=c[r+8>>2]|0;if((v|0)>=(u|0)){y=q;break}A=v+1|0;Nn((c[m>>2]|0)+(v<<2)|0,0,((u|0)>(A|0)?u:A)-v<<2|0)|0;y=q}}while(0);o=c[r+24>>2]|0;if((o|0)==0){x=y;break}else{p=y;r=o}}}r=(w|0)>0;if(r){y=c[b+124>>2]|0;p=0;do{m=e[i+(p*20|0)+2>>1]|0;F=+g[i+(p*20|0)+4>>2];z=y+((e[i+(p*20|0)>>1]|0)<<2)|0;g[z>>2]=F+ +g[z>>2];z=y+(m<<2)|0;g[z>>2]=F+ +g[z>>2];p=p+1|0;}while((p|0)<(w|0))}p=(x|0)>0;if(p){y=b+124|0;z=b+132|0;m=0;while(1){n=c[t+(m<<2)>>2]|0;E=c[n+4>>2]|0;D=c[n+8>>2]|0;if((E|0)<(D|0)){n=c[y>>2]|0;f=c[z>>2]|0;l=E;do{g[f+(l<<2)>>2]=+g[n+(l<<2)>>2]<.800000011920929?0.0:3.4028234663852886e+38;l=l+1|0;}while((l|0)<(D|0))}D=m+1|0;if((D|0)<(x|0)){m=D}else{G=z;break}}}else{G=b+132|0}z=~~+R(+(c[b+44>>2]|0));a:do{if(r){m=0;while(1){if((m|0)>=(z|0)){break a}y=c[G>>2]|0;D=0;l=0;while(1){F=1.0- +g[i+(D*20|0)+4>>2];n=y+((e[i+(D*20|0)>>1]|0)<<2)|0;f=y+((e[i+(D*20|0)+2>>1]|0)<<2)|0;H=+g[f>>2];I=F+H;J=+g[n>>2];K=F+J;if(J>I){g[n>>2]=I;L=1;M=+g[f>>2]}else{L=l;M=H}if(M>K){g[f>>2]=K;N=1}else{N=L}f=D+1|0;if((f|0)<(w|0)){D=f;l=N}else{break}}if(N){m=m+1|0}else{break}}}}while(0);if(!p){O=c[d>>2]|0;P=O+76|0;qn(P,s);Q=c[d>>2]|0;S=Q+76|0;qn(S,h);return}p=b+32|0;b=0;do{N=c[t+(b<<2)>>2]|0;w=c[N+4>>2]|0;L=N+8|0;if((w|0)<(c[L>>2]|0)){N=c[G>>2]|0;i=w;do{w=N+(i<<2)|0;M=+g[w>>2];if(M<3.4028234663852886e+38){T=M*+g[p>>2]}else{T=0.0}g[w>>2]=T;i=i+1|0;}while((i|0)<(c[L>>2]|0))}b=b+1|0;}while((b|0)<(x|0));O=c[d>>2]|0;P=O+76|0;qn(P,s);Q=c[d>>2]|0;S=Q+76|0;qn(S,h);return}function al(a,b,d){a=a|0;b=b|0;d=d|0;var e=0.0,f=0,h=0,i=0,j=0,k=0,l=0,m=0;e=+g[b+36>>2];f=(~~(e*+g[d+4>>2]+-1.0+2048.0)<<20)+~~((e*+g[d>>2]+-1.0)*256.0+524288.0)|0;h=~~(e*+g[d+12>>2]+1.0+2048.0);i=~~((e*+g[d+8>>2]+1.0)*256.0+524288.0);d=c[b+200>>2]|0;j=c[b+204>>2]|0;b=d+(j<<3)|0;k=d;d=j<<3>>3;a:while(1){j=d;while(1){if((j|0)==0){break a}l=(j|0)/2|0;if((c[k+(l<<3)+4>>2]|0)>>>0<f>>>0){break}else{j=l}}k=k+(l+1<<3)|0;d=j-1-l|0}l=(h<<20)+i|0;i=k;h=b-k>>3;b:while(1){b=h;while(1){if((b|0)==0){break b}m=(b|0)/2|0;if((c[i+(m<<3)+4>>2]|0)>>>0>l>>>0){b=m}else{break}}i=i+(m+1<<3)|0;h=b-1-m|0}c[a>>2]=f&1048575;c[a+4>>2]=l&1048575;c[a+8>>2]=f&-1048576;c[a+12>>2]=l&-1048576;c[a+16>>2]=k;c[a+20>>2]=i;return}function bl(a,d,e,f){a=a|0;d=d|0;e=e|0;f=f|0;var h=0,i=0.0,j=0.0,l=0.0,m=0.0,n=0.0,o=0,p=0,q=0,r=0,s=0,t=0,u=0,v=0,w=0,x=0;h=c[a+96>>2]|0;i=+g[h+(e<<3)>>2]- +g[h+(d<<3)>>2];j=+g[h+(e<<3)+4>>2]- +g[h+(d<<3)+4>>2];l=i*i+j*j;if(!(l<+g[a+40>>2])){return}m=(c[k>>2]=1597463007-((g[k>>2]=l,c[k>>2]|0)>>1),+g[k>>2]);n=m*(1.5-m*l*.5*m);h=f+4|0;o=c[h>>2]|0;p=f+8|0;q=c[p>>2]|0;if((o|0)<(q|0)){r=o;s=c[f>>2]|0}else{t=(o|0)==0?256:o<<1;o=f+12|0;u=Vm(c[o>>2]|0,t*20|0)|0;v=u;w=f|0;f=c[w>>2]|0;if((f|0)!=0){x=q*20|0;On(u|0,f|0,x)|0;Wm(c[o>>2]|0,c[w>>2]|0,x)}c[p>>2]=t;c[w>>2]=v;r=c[h>>2]|0;s=v}c[h>>2]=r+1;b[s+(r*20|0)>>1]=d;b[s+(r*20|0)+2>>1]=e;h=c[a+88>>2]|0;c[s+(r*20|0)+16>>2]=c[h+(e<<2)>>2]|c[h+(d<<2)>>2];g[s+(r*20|0)+4>>2]=1.0-l*n*+g[a+36>>2];a=s+(r*20|0)+8|0;l=+(i*n);i=+(j*n);g[a>>2]=l;g[a+4>>2]=i;return}function cl(a,b){a=a|0;b=b|0;var d=0,f=0,g=0,h=0,j=0,k=0,l=0,m=0,n=0,o=0,p=0,q=0,r=0,s=0,t=0,u=0,v=0,w=0;d=i;f=b;b=i;i=i+8|0;c[b>>2]=c[f>>2];c[b+4>>2]=c[f+4>>2];f=a|0;g=c[f>>2]|0;h=a+4|0;a=c[h>>2]|0;j=g+(a*20|0)|0;k=b;b=c[k>>2]|0;l=c[k+4>>2]|0;k=l;a:do{if((a|0)==0){m=g}else{n=l;o=g;while(1){if((c[o+16>>2]&131072|0)!=0){if(!(qb[c[(c[n>>2]|0)+16>>2]&15](k,b,e[o>>1]|0,e[o+2>>1]|0)|0)){m=o;break a}}p=o+20|0;if((p|0)==(j|0)){q=j;break}else{o=p}}r=c[f>>2]|0;s=q;t=r;u=s-t|0;v=(u|0)/20|0;c[h>>2]=v;i=d;return q|0}}while(0);if((m|0)==(j|0)){q=j;r=c[f>>2]|0;s=q;t=r;u=s-t|0;v=(u|0)/20|0;c[h>>2]=v;i=d;return q|0}g=l;l=m;a=m;b:while(1){m=l;while(1){w=m+20|0;if((w|0)==(j|0)){q=a;break b}if((c[m+36>>2]&131072|0)==0){break}if(qb[c[(c[g>>2]|0)+16>>2]&15](k,b,e[w>>1]|0,e[m+22>>1]|0)|0){break}else{m=w}}m=a;o=w;c[m>>2]=c[o>>2];c[m+4>>2]=c[o+4>>2];c[m+8>>2]=c[o+8>>2];c[m+12>>2]=c[o+12>>2];c[m+16>>2]=c[o+16>>2];l=w;a=a+20|0}r=c[f>>2]|0;s=q;t=r;u=s-t|0;v=(u|0)/20|0;c[h>>2]=v;i=d;return q|0}function dl(a,d,e,f){a=a|0;d=d|0;e=e|0;f=f|0;var g=0,h=0,j=0,k=0,l=0,m=0,n=0,o=0,p=0,q=0,r=0;g=i;i=i+8|0;h=g|0;j=a|0;k=c[j>>2]|0;if((k|0)!=0){qn(c[a+12>>2]|0,k);c[j>>2]=0;c[a+8>>2]=0}if((e|0)==0){l=c[a+8>>2]|0}else{k=on(c[a+12>>2]|0,e*9|0)|0;c[j>>2]=k;m=k+(e<<3)|0;c[a+4>>2]=m;Nn(m|0,1,e|0)|0;c[a+8>>2]=e;l=e}if((l|0)==0){i=g;return}l=c[j>>2]|0;if((e|0)>0){j=0;m=0;while(1){k=b[d+(j*20|0)>>1]|0;n=k&65535;do{if(k<<16>>16==-1){o=m}else{p=b[d+(j*20|0)+2>>1]|0;if(p<<16>>16==-1){o=m;break}q=p&65535;if(((c[f+(q<<2)>>2]|c[f+(n<<2)>>2])&32768|0)==0){o=m;break}c[l+(j<<3)>>2]=n;c[l+(j<<3)+4>>2]=q;o=m+1|0}}while(0);n=j+1|0;if((n|0)<(e|0)){j=n;m=o}else{r=o;break}}}else{r=0}c[a+8>>2]=r;c[h>>2]=28;Pl(l,l+(r<<3)|0,h);i=g;return}function el(b,d){b=b|0;d=d|0;var f=0,g=0,h=0,i=0,j=0,k=0,l=0,m=0,n=0,o=0,p=0,q=0,r=0,s=0,t=0,u=0,v=0,w=0,x=0,y=0,z=0,A=0,B=0,C=0,D=0,E=0,F=0,G=0;if((c[b+8>>2]&32768|0)==0){return}f=c[(c[b+400>>2]|0)+102952>>2]|0;if((f|0)==0){return}g=c[b+220>>2]|0;h=c[b+216>>2]|0;i=h+(g*20|0)|0;j=d+8|0;if((g|0)>0){g=f;k=d|0;l=d+4|0;m=h;while(1){h=e[m>>1]|0;n=e[m+2>>1]|0;o=c[j>>2]|0;do{if((o|0)==0){p=24}else{q=c[k>>2]|0;r=q;s=q;q=r+(o<<3)-s>>3;t=(q|0)==0;a:do{if(t){u=r}else{v=q;w=r;while(1){x=v;while(1){y=(x|0)/2|0;if((c[w+(y<<3)>>2]|0)<(h|0)){if((c[w+(y<<3)+4>>2]|0)<(n|0)){break}}if((x+1|0)>>>0<3>>>0){u=w;break a}else{x=y}}z=w+(y+1<<3)|0;A=x-1|0;if((A|0)==(y|0)){u=z;break}else{v=A-y|0;w=z}}}}while(0);if((u|0)==0){p=15}else{w=(u-s|0)>>>3;v=c[l>>2]|0;if((a[v+w|0]|0)==0){p=15}else{B=w;C=v}}if((p|0)==15){p=0;b:do{if(t){D=r}else{v=q;w=r;while(1){z=v;while(1){E=(z|0)/2|0;if((c[w+(E<<3)>>2]|0)<(n|0)){if((c[w+(E<<3)+4>>2]|0)<(h|0)){break}}if((z+1|0)>>>0<3>>>0){D=w;break b}else{z=E}}x=w+(E+1<<3)|0;A=z-1|0;if((A|0)==(E|0)){D=x;break}else{v=A-E|0;w=x}}}}while(0);if((D|0)==0){p=24;break}r=(D-s|0)>>>3;q=c[l>>2]|0;if((a[q+r|0]|0)==0){p=24;break}else{B=r;C=q}}a[C+B|0]=0}}while(0);if((p|0)==24){p=0;ob[c[(c[g>>2]|0)+24>>2]&31](f,b,m)}h=m+20|0;if(h>>>0<i>>>0){m=h}else{F=k;G=l;break}}}else{F=d|0;G=d+4|0}d=c[j>>2]|0;j=c[F>>2]|0;F=c[G>>2]|0;if((d|0)<=0){return}G=f;l=0;do{if((a[F+l|0]|0)!=0){wb[c[(c[G>>2]|0)+28>>2]&63](f,b,c[j+(l<<3)>>2]|0,c[j+(l<<3)+4>>2]|0)}l=l+1|0;}while((l|0)<(d|0));return}function fl(a,b){a=a|0;b=b|0;var d=0,e=0,f=0,g=0,h=0,i=0,j=0,k=0,l=0,m=0,n=0,o=0,p=0,q=0,r=0,s=0;d=a+156|0;if((c[d>>2]|0)<1){return}e=(c[a+176>>2]|0)+(b<<2)|0;f=(c[a+160>>2]|0)+(b<<2)|0;g=(c[a+168>>2]|0)+(b<<2)|0;h=(c[g>>2]|0)+1|0;c[g>>2]=h;do{if((h|0)==2){g=(c[e>>2]|0)+1|0;c[e>>2]=g;if((g|0)<=(c[d>>2]|0)){break}g=a+184|0;i=a+188|0;j=c[i>>2]|0;k=a+192|0;l=c[k>>2]|0;if((j|0)<(l|0)){m=j;n=c[g>>2]|0}else{o=(j|0)==0?256:j<<1;j=a+196|0;p=Vm(c[j>>2]|0,o<<2)|0;q=p;r=g|0;g=c[r>>2]|0;if((g|0)!=0){s=l<<2;On(p|0,g|0,s)|0;Wm(c[j>>2]|0,c[r>>2]|0,s)}c[k>>2]=o;c[r>>2]=q;m=c[i>>2]|0;n=q}c[i>>2]=m+1;c[n+(m<<2)>>2]=b}}while(0);c[f>>2]=c[a+4>>2];return}function gl(a,b){a=a|0;b=b|0;var d=0;if((c[a>>2]|0)>=(c[b>>2]|0)){d=0;return d|0}d=(c[a+4>>2]|0)<(c[b+4>>2]|0);return d|0}function hl(b){b=b|0;var d=0,e=0,f=0,h=0,j=0,k=0,l=0,m=0,n=0,o=0,p=0,q=0,r=0,s=0,t=0,u=0,v=0,w=0,x=0,y=0,z=0,A=0,B=0,C=0,D=0,E=0,F=0,G=0,H=0,I=0.0,J=0.0,K=0.0,L=0.0,M=0.0,N=0.0,O=0.0,P=0.0,Q=0.0,R=0.0,S=0.0,T=0.0,U=0.0,V=0.0,W=0,X=0,Y=0,Z=0;d=i;i=i+96|0;e=d|0;f=d+8|0;h=d+16|0;j=d+32|0;k=d+40|0;l=d+56|0;m=d+80|0;n=m|0;o=m;p=b+8|0;q=c[b+400>>2]|0;if((c[p>>2]&16384|0)==0){r=0}else{r=c[q+102952>>2]|0}s=b+400|0;t=k|0;c[t>>2]=0;u=k+4|0;c[u>>2]=0;v=k+8|0;c[v>>2]=0;w=k+12|0;c[w>>2]=q+76;q=(r|0)!=0;do{if(q){x=c[b+232>>2]|0;y=c[b+236>>2]|0;z=c[b+88>>2]|0;if((y|0)==0){break}A=on(c[w>>2]|0,y*9|0)|0;c[t>>2]=A;B=A+(y<<3)|0;c[u>>2]=B;Nn(B|0,1,y|0)|0;c[v>>2]=y;B=c[t>>2]|0;if((y|0)>0){A=0;C=0;while(1){D=c[x+(A*28|0)>>2]|0;do{if((D|0)==-1){E=C}else{if((c[z+(D<<2)>>2]&16384|0)==0){E=C;break}c[B+(A<<3)>>2]=c[x+(A*28|0)+8>>2];c[B+(A<<3)+4>>2]=D;E=C+1|0}}while(0);D=A+1|0;if((D|0)<(y|0)){A=D;C=E}else{F=E;break}}}else{F=0}c[v>>2]=F;c[e>>2]=40;om(B,B+(F<<3)|0,e)}}while(0);do{if((c[b+156>>2]|0)>0){e=c[b+44>>2]|0;if((e|0)<=0){break}F=c[b+168>>2]|0;E=b+4|0;C=c[b+160>>2]|0;A=b+176|0;y=0;do{c[F+(y<<2)>>2]=0;if((c[E>>2]|0)>((c[C+(y<<2)>>2]|0)+1|0)){c[(c[A>>2]|0)+(y<<2)>>2]=0}y=y+1|0;}while((y|0)<(e|0))}}while(0);e=b+236|0;c[e>>2]=0;c[b+188>>2]=0;y=c[s>>2]|0;s=c[p>>2]|0;if((s&65536|0)==0){G=0}else{G=c[y+102948>>2]|0}if((s&16384|0)==0){H=0}else{H=c[y+102952>>2]|0}c[l+4>>2]=b;c[l>>2]=4360;c[l+8>>2]=y;c[l+12>>2]=G;c[l+16>>2]=H;c[l+20>>2]=k;k=c[b+44>>2]|0;H=m;g[H>>2]=3.4028234663852886e+38;G=o+4|0;g[G>>2]=3.4028234663852886e+38;s=m+8|0;m=s;g[m>>2]=-3.4028234663852886e+38;p=o+12|0;g[p>>2]=-3.4028234663852886e+38;if((k|0)>0){A=c[b+96>>2]|0;C=0;I=3.4028234663852886e+38;J=3.4028234663852886e+38;K=-3.4028234663852886e+38;L=-3.4028234663852886e+38;while(1){E=A+(C<<3)|0;M=+g[E>>2];N=+g[E+4>>2];O=I<M?I:M;P=J<N?J:N;Q=+O;R=+P;g[n>>2]=Q;g[n+4>>2]=R;R=K>M?K:M;M=L>N?L:N;N=+R;Q=+M;g[s>>2]=N;g[s+4>>2]=Q;E=C+1|0;if((E|0)<(k|0)){C=E;I=O;J=P;K=R;L=M}else{S=O;T=P;U=R;V=M;break}}}else{S=3.4028234663852886e+38;T=3.4028234663852886e+38;U=-3.4028234663852886e+38;V=-3.4028234663852886e+38}L=+g[b+32>>2];g[H>>2]=S-L;g[G>>2]=T-L;g[m>>2]=U+L;g[p>>2]=V+L;Sj(y,l|0,o);if((a[b+316|0]|0)!=0){o=b+232|0;l=c[o>>2]|0;y=l+((c[e>>2]|0)*28|0)|0;c[f>>2]=48;Ll(l,y,f);c[j>>2]=0;f=c[o>>2]|0;o=f+((c[e>>2]|0)*28|0)|0;c[h>>2]=b;c[h+4>>2]=-1;c[h+8>>2]=0;c[h+12>>2]=j;y=f;while(1){if((y|0)==(o|0)){break}if(Kl(h,y)|0){W=y;X=y;Y=33;break}else{y=y+28|0}}a:do{if((Y|0)==33){while(1){Y=0;y=W;do{y=y+28|0;if((y|0)==(o|0)){break a}}while(Kl(h,y)|0);f=X;l=y;c[f>>2]=c[l>>2];c[f+4>>2]=c[l+4>>2];c[f+8>>2]=c[l+8>>2];c[f+12>>2]=c[l+12>>2];c[f+16>>2]=c[l+16>>2];c[f+20>>2]=c[l+20>>2];c[f+24>>2]=c[l+24>>2];W=y;X=X+28|0;Y=33}}}while(0);c[e>>2]=(c[e>>2]|0)-(c[j>>2]|0)}do{if(q){j=c[t>>2]|0;e=j;X=c[u>>2]|0;W=c[v>>2]|0;if((W|0)<=0){Z=j;break}j=r;h=0;while(1){if((a[X+h|0]|0)!=0){wb[c[(c[j>>2]|0)+20>>2]&63](r,c[e+(h<<3)>>2]|0,b,c[e+(h<<3)+4>>2]|0)}o=h+1|0;if((o|0)<(W|0)){h=o}else{Y=50;break}}}else{Y=50}}while(0);if((Y|0)==50){Z=c[t>>2]|0}if((Z|0)==0){i=d;return}qn(c[w>>2]|0,Z);c[t>>2]=0;c[v>>2]=0;i=d;return}function il(a,b){a=a|0;b=b|0;var d=0,e=0,f=0;d=c[a>>2]|0;e=c[b>>2]|0;if((d|0)==(e|0)){f=+g[a+12>>2]>+g[b+12>>2];return f|0}else{f=(d|0)<(e|0);return f|0}return 0}function jl(a,b){a=a|0;b=b|0;var d=0,e=0,f=0,h=0,j=0,k=0,l=0,m=0,n=0,o=0,p=0,q=0,r=0,s=0,t=0,u=0,v=0.0,w=0,x=0.0,y=0.0,z=0.0,A=0.0,B=0,C=0.0,D=0.0,E=0.0,F=0.0,G=0.0,H=0.0;d=i;i=i+16|0;e=d|0;f=e|0;h=e;j=i;i=i+36|0;i=i+7&-8;g[e>>2]=3.4028234663852886e+38;g[h+4>>2]=3.4028234663852886e+38;k=e+8|0;g[k>>2]=-3.4028234663852886e+38;g[h+12>>2]=-3.4028234663852886e+38;e=c[a+44>>2]|0;if((e|0)<=0){l=j|0;m=j+4|0;c[m>>2]=a;c[l>>2]=4440;n=j+8|0;o=n;p=b;c[o>>2]=c[p>>2];c[o+4>>2]=c[p+4>>2];c[o+8>>2]=c[p+8>>2];c[o+12>>2]=c[p+12>>2];c[o+16>>2]=c[p+16>>2];c[o+20>>2]=c[p+20>>2];c[o+24>>2]=c[p+24>>2];q=a+400|0;r=c[q>>2]|0;s=j|0;Sj(r,s,h);i=d;return}t=c[a+104>>2]|0;u=c[a+96>>2]|0;v=+g[b>>2];w=0;x=3.4028234663852886e+38;y=3.4028234663852886e+38;z=-3.4028234663852886e+38;A=-3.4028234663852886e+38;do{B=t+(w<<3)|0;C=+g[B>>2];D=+g[B+4>>2];B=u+(w<<3)|0;E=+g[B>>2];F=+g[B+4>>2];G=E+C*v;C=D*v+F;D=E<G?E:G;H=F<C?F:C;x=x<D?x:D;y=y<H?y:H;H=+x;D=+y;g[f>>2]=H;g[f+4>>2]=D;D=E>G?E:G;G=F>C?F:C;z=z>D?z:D;A=A>G?A:G;G=+z;D=+A;g[k>>2]=G;g[k+4>>2]=D;w=w+1|0;}while((w|0)<(e|0));l=j|0;m=j+4|0;c[m>>2]=a;c[l>>2]=4440;n=j+8|0;o=n;p=b;c[o>>2]=c[p>>2];c[o+4>>2]=c[p+4>>2];c[o+8>>2]=c[p+8>>2];c[o+12>>2]=c[p+12>>2];c[o+16>>2]=c[p+16>>2];c[o+20>>2]=c[p+20>>2];c[o+24>>2]=c[p+24>>2];q=a+400|0;r=c[q>>2]|0;s=j|0;Sj(r,s,h);i=d;return}function kl(b,d){b=b|0;d=d|0;var e=0,f=0,h=0,j=0,k=0,l=0,m=0,n=0,o=0,p=0,q=0,r=0,s=0,t=0,u=0,v=0,w=0,x=0.0,y=0,z=0.0,A=0.0,B=0,C=0,D=0,E=0,F=0,G=0,H=0,I=0,J=0.0,K=0.0,L=0.0,M=0.0,N=0,O=0,P=0,Q=0,S=0,T=0,U=0,V=0,W=0.0,X=0.0,Y=0.0,Z=0.0,_=0.0,$=0.0,aa=0.0,ba=0.0,ca=0,da=0.0,ea=0.0,fa=0.0,ga=0.0,ha=0.0,ia=0.0,ja=0.0,ka=0.0,la=0.0,ma=0.0,na=0.0,oa=0.0,pa=0.0,qa=0;e=i;i=i+16|0;f=e|0;h=f|0;j=f;k=i;i=i+24|0;l=b+44|0;m=c[l>>2]|0;if((m|0)>0){n=b+88|0;o=b+144|0;p=b+104|0;q=b+96|0;r=0;s=m;while(1){m=c[(c[n>>2]|0)+(r<<2)>>2]|0;do{if((m&1024|0)==0){t=s}else{if((m&4|0)!=0){u=c[p>>2]|0;g[u+(r<<3)>>2]=0.0;g[u+(r<<3)+4>>2]=0.0;t=s;break}u=c[(c[o>>2]|0)+(r<<2)>>2]|0;if((c[u+12>>2]&2|0)==0){t=s;break}v=(c[p>>2]|0)+(r<<3)|0;w=c[q>>2]|0;Ck(u);x=+g[u+56>>2];y=v;z=+(+g[u+48>>2]+(+g[w+(r<<3)+4>>2]- +g[u+44>>2])*(-0.0-x));A=+(x*(+g[w+(r<<3)>>2]- +g[u+40>>2])+ +g[u+52>>2]);g[y>>2]=z;g[y+4>>2]=A;t=c[l>>2]|0}}while(0);m=r+1|0;if((m|0)<(t|0)){r=m;s=t}else{break}}}A=+g[d>>2]*2.5;t=b+252|0;if((c[t>>2]|0)<=0){i=e;return}s=b+248|0;r=b+96|0;q=f+8|0;f=k+16|0;p=k+20|0;o=k|0;n=k+4|0;m=b+144|0;y=b+104|0;u=d+4|0;d=b+32|0;w=b+320|0;v=b+88|0;B=b+21|0;C=b+112|0;D=0;do{E=c[s>>2]|0;a:do{if((c[E+(D*20|0)+8>>2]&1024|0)!=0){F=c[E+(D*20|0)>>2]|0;G=c[E+(D*20|0)+4>>2]|0;H=c[r>>2]|0;I=H+(F<<3)|0;z=+g[I>>2];x=+g[I+4>>2];I=H+(G<<3)|0;J=+g[I>>2];K=+g[I+4>>2];L=+(z<J?z:J);M=+(x<K?x:K);g[h>>2]=L;g[h+4>>2]=M;M=+(z>J?z:J);L=+(x>K?x:K);g[q>>2]=M;g[q+4>>2]=L;al(k,b,j);I=c[f>>2]|0;H=c[p>>2]|0;if(!(I>>>0<H>>>0)){break}L=J-z;J=K-x;N=c[o>>2]|0;O=c[n>>2]|0;P=I;while(1){I=c[P+4>>2]&1048575;Q=P+8|0;c[f>>2]=Q;if(I>>>0<N>>>0|I>>>0>O>>>0){if(Q>>>0<H>>>0){P=Q;continue}else{break}}I=c[P>>2]|0;if(!((I|0)>-1)){break a}S=(c[r>>2]|0)+(I<<3)|0;K=+g[S>>2];M=+g[S+4>>2];S=c[m>>2]|0;T=c[S+(I<<2)>>2]|0;b:do{if((c[S+(F<<2)>>2]|0)!=(T|0)){if((c[S+(G<<2)>>2]|0)==(T|0)){break}U=c[y>>2]|0;V=U+(F<<3)|0;W=+g[V>>2];X=+g[V+4>>2];V=U+(G<<3)|0;Y=+g[V>>2];Z=K-z;_=M-x;$=Y-W;Y=+g[V+4>>2]-X;V=U+(I<<3)|0;aa=+g[V>>2];ba=aa-W;ca=U+(I<<3)+4|0;da=+g[ca>>2];ea=da-X;fa=$*ea-ba*Y;ga=L*ea-J*ba-(Z*Y-_*$);ha=L*_-J*Z;do{if(fa==0.0){if(ga==0.0){break b}ia=(-0.0-ha)/ga;if(!(ia>=0.0&ia<A)){break b}ja=L+$*ia;ka=J+Y*ia;la=(ja*(Z+ba*ia)+ka*(_+ea*ia))/(ja*ja+ka*ka);if(la>=0.0&la<=1.0){ma=la}else{break b}}else{la=ga*ga-ha*4.0*fa;if(la<0.0){break b}ka=+R(la);la=fa*2.0;ja=(-0.0-ga-ka)/la;ia=(ka-ga)/la;U=ja>ia;la=U?ia:ja;ka=U?ja:ia;ia=L+$*la;ja=J+Y*la;na=(ia*(Z+ba*la)+ja*(_+ea*la))/(ia*ia+ja*ja);if(la>=0.0&la<A){if(na>=0.0&na<=1.0){ma=na;break}}if(!(ka>=0.0&ka<A)){break b}na=L+$*ka;la=J+Y*ka;ja=(na*(Z+ba*ka)+la*(_+ea*ka))/(na*na+la*la);if(ja>=0.0&ja<=1.0){ma=ja}else{break b}}}while(0);ea=W+$*ma-aa;_=X+Y*ma-da;ba=+g[d>>2]*.75;Z=ba*+g[w>>2]*ba*(-0.0- +g[u>>2]);ba=ea*Z;ga=_*Z;do{if(ba!=0.0|ga!=0.0){if((c[(c[v>>2]|0)+(I<<2)>>2]&4|0)!=0){oa=aa;pa=da;break}if((a[B]|0)==0){Nn(c[C>>2]|0,0,c[l>>2]<<3|0)|0;a[B]=1}U=c[C>>2]|0;qa=U+(I<<3)|0;g[qa>>2]=ba+ +g[qa>>2];qa=U+(I<<3)+4|0;g[qa>>2]=ga+ +g[qa>>2];oa=+g[V>>2];pa=+g[ca>>2]}else{oa=aa;pa=da}}while(0);g[V>>2]=ea+oa;g[ca>>2]=_+pa}}while(0);if(Q>>>0<H>>>0){P=Q}else{break}}}}while(0);D=D+1|0;}while((D|0)<(c[t>>2]|0));i=e;return}function ll(b,d){b=b|0;d=d|0;var f=0,h=0,j=0,k=0,l=0,m=0,n=0,o=0,p=0,q=0,r=0,s=0,t=0,u=0,v=0,w=0,x=0,y=0,z=0,A=0,B=0,C=0,D=0,E=0,F=0,G=0,H=0,I=0,J=0,K=0,L=0,M=0.0,N=0.0,O=0,P=0,Q=0,S=0,T=0,U=0,V=0.0,W=0,X=0,Y=0.0,Z=0,_=0,$=0,aa=0.0,ba=0;f=i;i=i+40|0;h=f|0;j=f+8|0;k=b+44|0;if((c[k>>2]|0)==0){i=f;return}if((c[b+280>>2]|0)!=0){ml(b,d)}l=b+8|0;if((c[l>>2]&2|0)!=0){Mk(b)}m=b+12|0;if((a[m]|0)!=0){c[l>>2]=0;n=c[k>>2]|0;if((n|0)>0){o=c[b+88>>2]|0;p=0;q=0;do{q=c[o+(p<<2)>>2]|q;c[l>>2]=q;p=p+1|0;}while((p|0)<(n|0))}a[m]=0}m=b+20|0;if((a[m]|0)!=0){n=b+16|0;c[n>>2]=0;p=c[b+312>>2]|0;if((p|0)!=0){q=p;p=0;do{p=c[q+12>>2]|p;c[n>>2]=p;q=c[q+24>>2]|0;}while((q|0)!=0)}a[m]=0}if((a[b|0]|0)!=0){i=f;return}m=b+24|0;c[m>>2]=0;q=d+20|0;if((c[q>>2]|0)<=0){i=f;return}p=b+4|0;n=j;o=d;d=j|0;r=j+4|0;s=b+116|0;t=b+236|0;u=b+232|0;v=b+220|0;w=b+216|0;x=b+16|0;y=b+21|0;z=b+324|0;A=b+400|0;B=b+104|0;C=b+32|0;D=b+88|0;E=b+96|0;F=b+368|0;G=b+36|0;H=b+28|0;I=b+112|0;J=h|0;K=h+4|0;L=h|0;do{c[p>>2]=(c[p>>2]|0)+1;c[n>>2]=c[o>>2];c[n+4>>2]=c[o+4>>2];c[n+8>>2]=c[o+8>>2];c[n+12>>2]=c[o+12>>2];c[n+16>>2]=c[o+16>>2];c[n+20>>2]=c[o+20>>2];c[n+24>>2]=c[o+24>>2];M=+(c[q>>2]|0);N=+g[d>>2]/M;g[d>>2]=N;g[r>>2]=+g[r>>2]*M;Vk(b,0);hl(b);Nn(c[s>>2]|0,0,c[k>>2]<<2|0)|0;h=c[t>>2]|0;if((h|0)>0){O=c[u>>2]|0;P=c[s>>2]|0;Q=0;do{S=P+(c[O+(Q*28|0)>>2]<<2)|0;g[S>>2]=+g[O+(Q*28|0)+12>>2]+ +g[S>>2];Q=Q+1|0;}while((Q|0)<(h|0))}h=c[v>>2]|0;if((h|0)>0){Q=c[w>>2]|0;O=c[s>>2]|0;P=0;do{S=e[Q+(P*20|0)+2>>1]|0;M=+g[Q+(P*20|0)+4>>2];T=O+(e[Q+(P*20|0)>>1]<<2)|0;g[T>>2]=M+ +g[T>>2];T=O+(S<<2)|0;g[T>>2]=M+ +g[T>>2];P=P+1|0;}while((P|0)<(h|0))}if((c[x>>2]&16|0)!=0){$k(b)}h=c[l>>2]|0;if((h&4096|0)==0){U=h}else{h=c[D>>2]|0;c[J>>2]=4192;c[K>>2]=h;Wk(b,0,c[k>>2]|0,L);if((c[k>>2]|0)>0){h=c[D>>2]|0;P=0;do{O=h+(P<<2)|0;c[O>>2]=c[O>>2]&-4097;P=P+1|0;}while((P|0)<(c[k>>2]|0))}P=c[l>>2]&-4097;c[l>>2]=P;U=P}if((a[y]|0)!=0){M=+g[G>>2]*1.3333333730697632;V=N*M*+g[H>>2]*M;P=c[k>>2]|0;if((P|0)>0){h=c[B>>2]|0;O=c[I>>2]|0;Q=0;do{M=V*+g[O+(Q<<3)+4>>2];T=h+(Q<<3)|0;g[T>>2]=V*+g[O+(Q<<3)>>2]+ +g[T>>2];T=h+(Q<<3)+4|0;g[T>>2]=M+ +g[T>>2];Q=Q+1|0;}while((Q|0)<(P|0))}a[y]=0}if((U&32|0)==0){W=U}else{nl(b);W=c[l>>2]|0}if((W&8192|0)==0){X=W}else{ol(b,j);X=c[l>>2]|0}do{if((X&64|0)!=0){V=+g[F>>2]*+g[C>>2]*+g[r>>2];P=c[v>>2]|0;if((P|0)<=0){break}Q=c[w>>2]|0;h=0;do{do{if((c[Q+(h*20|0)+16>>2]&64|0)!=0){N=+g[Q+(h*20|0)+4>>2];if(!(N>.25)){break}O=e[Q+(h*20|0)>>1]|0;T=e[Q+(h*20|0)+2>>1]|0;S=Q+(h*20|0)+8|0;M=+g[S>>2];Y=V*(N+-.25);N=Y*M;M=Y*+g[S+4>>2];S=c[B>>2]|0;Z=S+(O<<3)|0;g[Z>>2]=+g[Z>>2]-N;Z=S+(O<<3)+4|0;g[Z>>2]=+g[Z>>2]-M;Z=S+(T<<3)|0;g[Z>>2]=N+ +g[Z>>2];Z=S+(T<<3)+4|0;g[Z>>2]=M+ +g[Z>>2]}}while(0);h=h+1|0;}while((h|0)<(P|0))}}while(0);if((X&128|0)!=0){pl(b,j)}if((c[x>>2]&1|0)!=0){ql(b,j)}if((c[l>>2]&256|0)!=0){rl(b)}V=+g[d>>2]*+g[z>>2];P=(c[A>>2]|0)+102980|0;M=+g[P>>2];N=V*M;M=V*+g[P+4>>2];P=c[k>>2]|0;if((P|0)>0){h=c[B>>2]|0;Q=0;do{Z=h+(Q<<3)|0;g[Z>>2]=N+ +g[Z>>2];Z=h+(Q<<3)+4|0;g[Z>>2]=M+ +g[Z>>2];Q=Q+1|0;}while((Q|0)<(P|0))}if((c[l>>2]&2048|0)!=0){sl(b,j)}tl(b,j);ul(b,j);P=c[l>>2]|0;if((P&2048|0)==0){_=P}else{vl(b);_=c[l>>2]|0}if((_&16|0)==0){$=_}else{wl(b,j);$=c[l>>2]|0}if(($&8|0)!=0){xl(b,j)}M=+g[C>>2]*+g[r>>2];N=M*M;P=c[k>>2]|0;if((P|0)>0){Q=c[B>>2]|0;h=0;do{Z=Q+(h<<3)|0;M=+g[Z>>2];T=Q+(h<<3)+4|0;V=+g[T>>2];Y=M*M+V*V;if(Y>N){aa=+R(N/Y);g[Z>>2]=M*aa;g[T>>2]=V*aa}h=h+1|0;}while((h|0)<(P|0))}if((c[x>>2]&2|0)!=0){yl(b)}if((c[l>>2]&1024|0)!=0){kl(b,j)}jl(b,j);if((c[x>>2]&2|0)!=0){zl(b,j)}P=c[k>>2]|0;do{if((c[l>>2]&4|0)==0){ba=81}else{if((P|0)<=0){break}h=c[D>>2]|0;Q=0;while(1){if((c[h+(Q<<2)>>2]&4|0)!=0){T=c[B>>2]|0;g[T+(Q<<3)>>2]=0.0;g[T+(Q<<3)+4>>2]=0.0}T=Q+1|0;if((T|0)<(P|0)){Q=T}else{ba=81;break}}}}while(0);do{if((ba|0)==81){ba=0;if((P|0)<=0){break}Q=c[E>>2]|0;N=+g[d>>2];h=c[B>>2]|0;T=0;do{aa=N*+g[h+(T<<3)+4>>2];Z=Q+(T<<3)|0;g[Z>>2]=N*+g[h+(T<<3)>>2]+ +g[Z>>2];Z=Q+(T<<3)+4|0;g[Z>>2]=aa+ +g[Z>>2];T=T+1|0;}while((T|0)<(P|0))}}while(0);P=(c[m>>2]|0)+1|0;c[m>>2]=P;}while((P|0)<(c[q>>2]|0));i=f;return}function ml(b,d){b=b|0;d=d|0;var e=0,f=0,h=0,j=0.0,k=0,l=0,m=0,n=0,o=0,p=0;e=i;i=i+8|0;f=e|0;h=b+296|0;j=+g[d>>2]/+g[b+396>>2]*4294967296.0;d=Rn(~~+j>>>0,(E=+j,+Q(E)>=1.0?E>0.0?(ga(+P(E/4294967296.0),4294967295.0)|0)>>>0:~~+aa((E- +(~~E>>>0))/4294967296.0)>>>0:0),c[h>>2]|0,c[h+4>>2]|0)|0;k=F;c[h>>2]=d;c[h+4>>2]=k;h=k;k=c[b+280>>2]|0;d=c[b+288>>2]|0;l=c[b+44>>2]|0;m=b+304|0;if((a[m]|0)!=0){c[f>>2]=k;Fl(d,d+(l<<2)|0,f);a[m]=0}if((l|0)<=0){i=e;return}m=b+88|0;f=l;while(1){l=f-1|0;n=c[d+(l<<2)>>2]|0;o=c[k+(n<<2)>>2]|0;if((h|0)<(o|0)|(o|0)<1){p=7;break}Pk(b,n,c[(c[m>>2]|0)+(n<<2)>>2]|2);if((l|0)>0){f=l}else{p=7;break}}if((p|0)==7){i=e;return}}function nl(a){a=a|0;var d=0.0,f=0,h=0,i=0,j=0,k=0,l=0,m=0,n=0,o=0,p=0,q=0,r=0.0,s=0.0,t=0.0,u=0,v=0,w=0,x=0,y=0,z=0,A=0.0,B=0,C=0.0,D=0.0,E=0.0,F=0.0,G=0;d=+g[a+352>>2];f=c[a+236>>2]|0;if((f|0)>0){h=a+96|0;i=a+104|0;j=a+36|0;k=a+28|0;l=c[a+232>>2]|0;m=c[a+88>>2]|0;n=0;do{o=c[l+(n*28|0)>>2]|0;do{if((c[m+(o<<2)>>2]&32|0)!=0){p=c[l+(n*28|0)+4>>2]|0;q=(c[h>>2]|0)+(o<<3)|0;r=+g[q>>2];s=+g[q+4>>2];q=p+88|0;t=+g[q>>2];u=p+60|0;v=p+64|0;w=p+80|0;x=p+84|0;y=c[i>>2]|0;z=y+(o<<3)|0;A=+g[z>>2];B=y+(o<<3)+4|0;C=+g[B>>2];D=+g[l+(n*28|0)+12>>2]*d*+g[l+(n*28|0)+24>>2];E=D*(+g[w>>2]+(s- +g[v>>2])*(-0.0-t)-A);F=D*(t*(r- +g[u>>2])+ +g[x>>2]-C);t=+g[j>>2]*1.3333333730697632;D=t*+g[k>>2]*t;g[z>>2]=A+E*D;g[B>>2]=C+F*D;D=-0.0-E;E=-0.0-F;if((c[p>>2]|0)!=2){break}B=p+4|0;z=b[B>>1]|0;if((z&2)==0){y=z|2;b[B>>1]=y;g[p+160>>2]=0.0;G=y}else{G=z}if((G&2)==0){break}F=+g[p+136>>2];g[w>>2]=F*D+ +g[w>>2];g[x>>2]=F*E+ +g[x>>2];g[q>>2]=+g[q>>2]+ +g[p+144>>2]*((r- +g[u>>2])*E-(s- +g[v>>2])*D)}}while(0);n=n+1|0;}while((n|0)<(f|0))}f=c[a+220>>2]|0;if((f|0)<=0){return}n=c[a+216>>2]|0;G=a+104|0;a=0;do{if((c[n+(a*20|0)+16>>2]&32|0)!=0){k=e[n+(a*20|0)>>1]|0;j=e[n+(a*20|0)+2>>1]|0;l=c[G>>2]|0;i=l+(j<<3)|0;h=l+(k<<3)|0;D=+g[h>>2];m=l+(j<<3)+4|0;j=l+(k<<3)+4|0;s=+g[j>>2];E=d*+g[n+(a*20|0)+4>>2];r=E*(+g[i>>2]-D);F=E*(+g[m>>2]-s);g[h>>2]=D+r;g[j>>2]=s+F;g[i>>2]=+g[i>>2]-r;g[m>>2]=+g[m>>2]-F}a=a+1|0;}while((a|0)<(f|0));return}function ol(a,b){a=a|0;b=b|0;var d=0.0,f=0,h=0,i=0,j=0,k=0,l=0,m=0.0,n=0.0,o=0.0,p=0;d=+g[a+364>>2]*+g[a+32>>2]*+g[b+4>>2];b=c[a+220>>2]|0;if((b|0)<=0){return}f=a+144|0;h=a+104|0;i=c[a+216>>2]|0;a=0;do{do{if((c[i+(a*20|0)+16>>2]&8192|0)!=0){j=e[i+(a*20|0)>>1]|0;k=e[i+(a*20|0)+2>>1]|0;l=c[f>>2]|0;if((c[l+(j<<2)>>2]|0)==(c[l+(k<<2)>>2]|0)){break}l=i+(a*20|0)+8|0;m=+g[l>>2];n=d*+g[i+(a*20|0)+4>>2];o=n*m;m=n*+g[l+4>>2];l=c[h>>2]|0;p=l+(j<<3)|0;g[p>>2]=+g[p>>2]-o;p=l+(j<<3)+4|0;g[p>>2]=+g[p>>2]-m;p=l+(k<<3)|0;g[p>>2]=o+ +g[p>>2];p=l+(k<<3)+4|0;g[p>>2]=m+ +g[p>>2]}}while(0);a=a+1|0;}while((a|0)<(b|0));return}function pl(a,b){a=a|0;b=b|0;var d=0,f=0,h=0,i=0,j=0,k=0,l=0,m=0,n=0.0,o=0,p=0.0,q=0.0,r=0,s=0.0,t=0.0,u=0.0,v=0.0;d=a+44|0;if((c[d>>2]|0)>0){f=a+128|0;h=0;do{i=(c[f>>2]|0)+(h<<3)|0;j=8600;k=c[j+4>>2]|0;c[i>>2]=c[j>>2];c[i+4>>2]=k;h=h+1|0;}while((h|0)<(c[d>>2]|0))}d=a+216|0;h=c[a+220>>2]|0;f=(h|0)>0;if(f){k=a+128|0;i=c[d>>2]|0;j=0;do{if((c[i+(j*20|0)+16>>2]&128|0)!=0){l=e[i+(j*20|0)>>1]|0;m=e[i+(j*20|0)+2>>1]|0;n=+g[i+(j*20|0)+4>>2];o=i+(j*20|0)+8|0;p=+g[o>>2];q=n*(1.0-n);n=q*p;p=q*+g[o+4>>2];o=c[k>>2]|0;r=o+(l<<3)|0;g[r>>2]=+g[r>>2]-n;r=o+(l<<3)+4|0;g[r>>2]=+g[r>>2]-p;r=o+(m<<3)|0;g[r>>2]=n+ +g[r>>2];r=o+(m<<3)+4|0;g[r>>2]=p+ +g[r>>2]}j=j+1|0;}while((j|0)<(h|0))}p=+g[a+32>>2]*+g[b+4>>2];n=+g[a+356>>2]*p;q=p*+g[a+360>>2];s=p*.5;if(!f){return}f=a+116|0;b=a+128|0;j=a+104|0;a=c[d>>2]|0;d=0;do{if((c[a+(d*20|0)+16>>2]&128|0)!=0){k=e[a+(d*20|0)>>1]|0;i=e[a+(d*20|0)+2>>1]|0;r=a+(d*20|0)+8|0;p=+g[r>>2];t=+g[r+4>>2];r=c[f>>2]|0;m=c[b>>2]|0;u=n*(+g[r+(k<<2)>>2]+ +g[r+(i<<2)>>2]+-2.0)+q*(p*(+g[m+(i<<3)>>2]- +g[m+(k<<3)>>2])+t*(+g[m+(i<<3)+4>>2]- +g[m+(k<<3)+4>>2]));v=+g[a+(d*20|0)+4>>2]*(u<s?u:s);u=p*v;p=t*v;m=c[j>>2]|0;r=m+(k<<3)|0;g[r>>2]=+g[r>>2]-u;r=m+(k<<3)+4|0;g[r>>2]=+g[r>>2]-p;r=m+(i<<3)|0;g[r>>2]=+g[r>>2]+u;r=m+(i<<3)+4|0;g[r>>2]=+g[r>>2]+p}d=d+1|0;}while((d|0)<(h|0));return}function ql(a,b){a=a|0;b=b|0;var d=0.0,f=0,h=0,i=0,j=0,k=0,l=0,m=0,n=0.0,o=0,p=0.0,q=0.0;d=+g[b+4>>2]*+g[a+372>>2];b=c[a+220>>2]|0;if((b|0)<=0){return}f=a+132|0;h=a+104|0;i=c[a+216>>2]|0;j=c[a+144>>2]|0;a=0;do{k=e[i+(a*20|0)>>1]|0;l=e[i+(a*20|0)+2>>1]|0;if((c[j+(k<<2)>>2]|0)!=(c[j+(l<<2)>>2]|0)){m=i+(a*20|0)+8|0;n=+g[m>>2];o=c[f>>2]|0;p=+g[i+(a*20|0)+4>>2]*d*(+g[o+(k<<2)>>2]+ +g[o+(l<<2)>>2]);q=n*p;n=+g[m+4>>2]*p;m=c[h>>2]|0;o=m+(k<<3)|0;g[o>>2]=+g[o>>2]-q;o=m+(k<<3)+4|0;g[o>>2]=+g[o>>2]-n;o=m+(l<<3)|0;g[o>>2]=q+ +g[o>>2];o=m+(l<<3)+4|0;g[o>>2]=n+ +g[o>>2]}a=a+1|0;}while((a|0)<(b|0));return}function rl(b){b=b|0;var f=0,h=0,i=0,j=0,k=0,l=0,m=0,n=0,o=0,p=0,q=0,r=0,s=0,t=0,u=0,v=0,w=0,x=0,y=0,z=0,A=0,B=0,C=0,D=0;f=~~(+g[b+388>>2]*128.0);if((f|0)==0){return}h=b+220|0;i=c[h>>2]|0;if((i|0)<=0){return}j=b+216|0;k=b+88|0;l=b+136|0;b=d[8432]|0;m=0;n=i;while(1){i=c[j>>2]|0;o=e[i+(m*20|0)>>1]|0;p=e[i+(m*20|0)+2>>1]|0;i=c[k>>2]|0;if((c[i+(o<<2)>>2]&256&c[i+(p<<2)>>2]|0)==0){q=n}else{i=c[l>>2]|0;r=i+(p<<2)|0;s=i+(o<<2)|0;t=d[s]|0;u=(ba((d[r]|0)-t|0,f)|0)>>b;v=i+(p<<2)+1|0;w=i+(o<<2)+1|0;x=d[w]|0;y=(ba((d[v]|0)-x|0,f)|0)>>b;z=i+(p<<2)+2|0;A=i+(o<<2)+2|0;B=d[A]|0;C=(ba((d[z]|0)-B|0,f)|0)>>b;D=i+(p<<2)+3|0;p=i+(o<<2)+3|0;o=d[p]|0;i=(ba((d[D]|0)-o|0,f)|0)>>b;a[s]=u+t;a[w]=y+x;a[A]=C+B;a[p]=i+o;a[r]=(d[r]|0)-u;a[v]=(d[v]|0)-y;a[z]=(d[z]|0)-C;a[D]=(d[D]|0)-i;q=c[h>>2]|0}i=m+1|0;if((i|0)<(q|0)){m=i;n=q}else{break}}return}function sl(a,b){a=a|0;b=b|0;var d=0,f=0,h=0,i=0,j=0,k=0,l=0.0,m=0.0,n=0.0,o=0,p=0,q=0,r=0,s=0,t=0,u=0,v=0.0,w=0,x=0,y=0,z=0.0;d=a+120|0;f=c[d>>2]|0;if((f|0)==0){h=a+48|0;i=c[h>>2]|0;if((i|0)==0){Kk(a,256);j=c[h>>2]|0}else{j=i}i=Vm(c[a+400>>2]|0,j<<2)|0;Nn(i|0,0,c[h>>2]<<2|0)|0;k=i}else{k=f}c[d>>2]=k;l=+g[a+32>>2]*+g[b+4>>2];m=+g[a+320>>2]*l*l;l=+g[a+376>>2]*m;n=m*.25;m=+g[a+380>>2];b=a+384|0;if((c[b>>2]|0)<=0){return}k=a+124|0;f=a+44|0;i=a+220|0;h=a+116|0;j=a+88|0;o=a+216|0;a=1;p=c[f>>2]|0;while(1){Nn(c[k>>2]|0,0,p<<2|0)|0;q=c[i>>2]|0;if((q|0)>0){r=c[o>>2]|0;s=0;do{if((c[r+(s*20|0)+16>>2]&2048|0)!=0){t=e[r+(s*20|0)>>1]|0;u=e[r+(s*20|0)+2>>1]|0;v=+g[r+(s*20|0)+4>>2];w=c[d>>2]|0;x=c[k>>2]|0;y=x+(t<<2)|0;g[y>>2]=v*+g[w+(u<<2)>>2]+ +g[y>>2];y=x+(u<<2)|0;g[y>>2]=v*+g[w+(t<<2)>>2]+ +g[y>>2]}s=s+1|0;}while((s|0)<(q|0))}q=c[f>>2]|0;if((q|0)>0){s=c[h>>2]|0;r=c[j>>2]|0;y=0;do{v=+g[s+(y<<2)>>2];if((c[r+(y<<2)>>2]&2048|0)==0){g[(c[d>>2]|0)+(y<<2)>>2]=0.0}else{z=(l*(v+-1.0)+ +g[(c[k>>2]|0)+(y<<2)>>2])/(m+v);v=z<n?z:n;g[(c[d>>2]|0)+(y<<2)>>2]=v<0.0?0.0:v}y=y+1|0;}while((y|0)<(q|0))}if((a|0)>=(c[b>>2]|0)){break}a=a+1|0;p=q}return}function tl(a,d){a=a|0;d=d|0;var f=0,h=0,i=0.0,j=0.0,k=0.0,l=0,m=0,n=0,o=0,p=0,q=0.0,r=0,s=0,t=0,u=0.0,v=0.0,w=0.0,x=0.0,y=0,z=0;f=a+320|0;h=a+32|0;i=+g[h>>2]*+g[d+4>>2];j=+g[f>>2]*i*i;i=+g[a+336>>2]*j;k=j*.25;l=c[a+44>>2]|0;m=(l|0)>0;if(m){n=c[a+116>>2]|0;o=c[a+124>>2]|0;p=0;do{j=+g[n+(p<<2)>>2]+-1.0;q=i*(j<0.0?0.0:j);g[o+(p<<2)>>2]=q<k?q:k;p=p+1|0;}while((p|0)<(l|0))}p=c[a+8>>2]|0;if(!((p&192|0)==0|m^1)){o=c[a+88>>2]|0;n=a+124|0;r=0;do{if((c[o+(r<<2)>>2]&192|0)!=0){g[(c[n>>2]|0)+(r<<2)>>2]=0.0}r=r+1|0;}while((r|0)<(l|0))}if(!((p&2048|0)==0|m^1)){m=c[a+88>>2]|0;p=a+120|0;r=a+124|0;n=0;do{if((c[m+(n<<2)>>2]&2048|0)!=0){o=(c[r>>2]|0)+(n<<2)|0;g[o>>2]=+g[(c[p>>2]|0)+(n<<2)>>2]+ +g[o>>2]}n=n+1|0;}while((n|0)<(l|0))}k=+g[d>>2]/(+g[f>>2]*+g[h>>2]);h=c[a+236>>2]|0;if((h|0)>0){f=a+36|0;d=a+28|0;l=c[a+232>>2]|0;n=c[a+96>>2]|0;p=c[a+124>>2]|0;r=c[a+104>>2]|0;m=0;do{o=c[l+(m*28|0)>>2]|0;s=c[l+(m*28|0)+4>>2]|0;q=+g[l+(m*28|0)+12>>2];t=l+(m*28|0)+16|0;j=+g[t>>2];u=+g[t+4>>2];t=n+(o<<3)|0;v=+g[t>>2];w=+g[t+4>>2];x=k*q*+g[l+(m*28|0)+24>>2]*(i*q+ +g[p+(o<<2)>>2]);q=j*x;j=u*x;x=+g[f>>2]*1.3333333730697632;u=x*+g[d>>2]*x;t=r+(o<<3)|0;g[t>>2]=+g[t>>2]-q*u;t=r+(o<<3)+4|0;g[t>>2]=+g[t>>2]-j*u;do{if((c[s>>2]|0)==2){t=s+4|0;o=b[t>>1]|0;if((o&2)==0){y=o|2;b[t>>1]=y;g[s+160>>2]=0.0;z=y}else{z=o}if((z&2)==0){break}u=+g[s+136>>2];o=s+80|0;g[o>>2]=q*u+ +g[o>>2];o=s+84|0;g[o>>2]=j*u+ +g[o>>2];o=s+88|0;g[o>>2]=+g[o>>2]+ +g[s+144>>2]*(j*(v- +g[s+60>>2])-q*(w- +g[s+64>>2]))}}while(0);m=m+1|0;}while((m|0)<(h|0))}h=c[a+220>>2]|0;if((h|0)<=0){return}m=c[a+216>>2]|0;z=c[a+124>>2]|0;r=c[a+104>>2]|0;a=0;do{d=e[m+(a*20|0)>>1]|0;f=e[m+(a*20|0)+2>>1]|0;p=m+(a*20|0)+8|0;i=+g[p>>2];w=k*+g[m+(a*20|0)+4>>2]*(+g[z+(d<<2)>>2]+ +g[z+(f<<2)>>2]);q=i*w;i=+g[p+4>>2]*w;p=r+(d<<3)|0;g[p>>2]=+g[p>>2]-q;p=r+(d<<3)+4|0;g[p>>2]=+g[p>>2]-i;p=r+(f<<3)|0;g[p>>2]=q+ +g[p>>2];p=r+(f<<3)+4|0;g[p>>2]=i+ +g[p>>2];a=a+1|0;}while((a|0)<(h|0));return}function ul(a,d){a=a|0;d=d|0;var f=0.0,h=0.0,i=0.0,j=0,k=0,l=0,m=0,n=0,o=0,p=0,q=0,r=0,s=0.0,t=0.0,u=0.0,v=0.0,w=0.0,x=0,y=0,z=0,A=0,B=0,C=0.0,D=0,E=0.0,F=0.0,G=0.0,H=0.0,I=0.0,J=0,K=0,L=0;f=+g[a+340>>2];h=1.0/(+g[a+32>>2]*+g[d+4>>2]);d=c[a+236>>2]|0;if((d|0)>0){i=-0.0-h;j=a+36|0;k=a+28|0;l=c[a+232>>2]|0;m=c[a+96>>2]|0;n=c[a+104>>2]|0;o=0;do{p=c[l+(o*28|0)>>2]|0;q=c[l+(o*28|0)+4>>2]|0;r=l+(o*28|0)+16|0;s=+g[r>>2];t=+g[r+4>>2];r=m+(p<<3)|0;u=+g[r>>2];v=+g[r+4>>2];r=q+88|0;w=+g[r>>2];x=q+60|0;y=q+64|0;z=q+80|0;A=q+84|0;B=n+(p<<3)|0;C=+g[B>>2];D=n+(p<<3)+4|0;E=+g[D>>2];F=s*(+g[z>>2]+(v- +g[y>>2])*(-0.0-w)-C)+t*(w*(u- +g[x>>2])+ +g[A>>2]-E);do{if(F<0.0){w=f*+g[l+(o*28|0)+12>>2];G=F*i;H=G<.5?G:.5;G=F*+g[l+(o*28|0)+24>>2]*(w>H?w:H);H=s*G;w=t*G;G=+g[j>>2]*1.3333333730697632;I=G*+g[k>>2]*G;g[B>>2]=C+I*H;g[D>>2]=E+I*w;I=-0.0-H;H=-0.0-w;if((c[q>>2]|0)!=2){break}p=q+4|0;J=b[p>>1]|0;if((J&2)==0){K=J|2;b[p>>1]=K;g[q+160>>2]=0.0;L=K}else{L=J}if((L&2)==0){break}w=+g[q+136>>2];g[z>>2]=w*I+ +g[z>>2];g[A>>2]=w*H+ +g[A>>2];g[r>>2]=+g[r>>2]+ +g[q+144>>2]*((u- +g[x>>2])*H-(v- +g[y>>2])*I)}}while(0);o=o+1|0;}while((o|0)<(d|0))}d=c[a+220>>2]|0;if((d|0)<=0){return}i=-0.0-h;o=c[a+216>>2]|0;L=c[a+104>>2]|0;a=0;do{k=e[o+(a*20|0)>>1]|0;j=e[o+(a*20|0)+2>>1]|0;l=o+(a*20|0)+8|0;h=+g[l>>2];v=+g[l+4>>2];l=L+(j<<3)|0;n=L+(k<<3)|0;u=+g[n>>2];m=L+(j<<3)+4|0;j=L+(k<<3)+4|0;E=+g[j>>2];C=h*(+g[l>>2]-u)+v*(+g[m>>2]-E);if(C<0.0){t=f*+g[o+(a*20|0)+4>>2];s=C*i;F=s<.5?s:.5;s=C*(t>F?t:F);F=h*s;h=v*s;g[n>>2]=u+F;g[j>>2]=E+h;g[l>>2]=+g[l>>2]-F;g[m>>2]=+g[m>>2]-h}a=a+1|0;}while((a|0)<(d|0));return}function vl(a){a=a|0;var d=0,e=0,f=0,h=0,i=0,j=0,k=0,l=0,m=0,n=0,o=0.0,p=0.0,q=0.0,r=0.0,s=0.0,t=0,u=0,v=0,w=0,x=0,y=0,z=0.0,A=0,B=0.0,C=0.0,D=0;d=c[a+236>>2]|0;if((d|0)<=0){return}e=a+96|0;f=a+104|0;h=a+36|0;i=a+28|0;j=c[a+232>>2]|0;k=c[a+88>>2]|0;a=0;do{l=c[j+(a*28|0)>>2]|0;do{if((c[k+(l<<2)>>2]&2048|0)!=0){m=c[j+(a*28|0)+4>>2]|0;n=j+(a*28|0)+16|0;o=+g[n>>2];p=+g[n+4>>2];n=(c[e>>2]|0)+(l<<3)|0;q=+g[n>>2];r=+g[n+4>>2];n=m+88|0;s=+g[n>>2];t=m+60|0;u=m+64|0;v=m+80|0;w=m+84|0;x=c[f>>2]|0;y=x+(l<<3)|0;z=+g[y>>2];A=x+(l<<3)+4|0;B=+g[A>>2];C=o*(+g[v>>2]+(r- +g[u>>2])*(-0.0-s)-z)+p*(s*(q- +g[t>>2])+ +g[w>>2]-B);if(!(C<0.0)){break}s=C*+g[j+(a*28|0)+24>>2]*.5;C=o*s;o=p*s;s=+g[h>>2]*1.3333333730697632;p=s*+g[i>>2]*s;g[y>>2]=z+C*p;g[A>>2]=B+o*p;p=-0.0-C;C=-0.0-o;if((c[m>>2]|0)!=2){break}A=m+4|0;y=b[A>>1]|0;if((y&2)==0){x=y|2;b[A>>1]=x;g[m+160>>2]=0.0;D=x}else{D=y}if((D&2)==0){break}o=+g[m+136>>2];g[v>>2]=o*p+ +g[v>>2];g[w>>2]=o*C+ +g[w>>2];g[n>>2]=+g[n>>2]+ +g[m+144>>2]*((q- +g[t>>2])*C-(r- +g[u>>2])*p)}}while(0);a=a+1|0;}while((a|0)<(d|0));return}function wl(a,b){a=a|0;b=b|0;var d=0.0,e=0,f=0,h=0,i=0,j=0,l=0,m=0,n=0,o=0,p=0.0,q=0.0,r=0.0,s=0.0,t=0.0,u=0.0,v=0,w=0.0,x=0,y=0.0,z=0.0,A=0,B=0,C=0.0,D=0.0,E=0.0,F=0.0,G=0,H=0.0,I=0.0,J=0.0;d=+g[b+4>>2]*+g[a+344>>2];e=c[a+268>>2]|0;if((e|0)<=0){return}f=a+96|0;h=a+104|0;i=b|0;b=c[a+264>>2]|0;a=0;do{if((c[b+(a*60|0)+12>>2]&16|0)!=0){j=c[b+(a*60|0)>>2]|0;l=c[b+(a*60|0)+4>>2]|0;m=c[b+(a*60|0)+8>>2]|0;n=c[f>>2]|0;o=n+(j<<3)|0;p=+g[o>>2];q=+g[o+4>>2];o=n+(l<<3)|0;r=+g[o>>2];s=+g[o+4>>2];o=n+(m<<3)|0;t=+g[o>>2];n=c[h>>2]|0;u=+g[i>>2];v=n+(j<<3)|0;w=+g[v>>2];x=n+(j<<3)+4|0;y=+g[x>>2];z=p+u*w;p=q+u*y;j=n+(l<<3)|0;A=n+(l<<3)+4|0;q=r+u*+g[j>>2];r=s+u*+g[A>>2];l=n+(m<<3)|0;B=n+(m<<3)+4|0;s=t+u*+g[l>>2];t=+g[o+4>>2]+u*+g[B>>2];u=(z+q+s)*.3333333432674408;C=(p+r+t)*.3333333432674408;D=z-u;z=p-C;p=q-u;q=r-C;r=s-u;u=t-C;C=+g[b+(a*60|0)+20>>2];t=+g[b+(a*60|0)+24>>2];o=b+(a*60|0)+28|0;s=+g[o>>2];m=b+(a*60|0)+32|0;E=+g[m>>2];n=b+(a*60|0)+36|0;F=+g[n>>2];G=b+(a*60|0)+40|0;H=+g[G>>2];I=C*z-t*D+(s*q-E*p)+(F*u-r*H);J=C*D+t*z+(s*p+E*q)+(r*F+u*H);H=I*I+J*J;F=(c[k>>2]=1597463007-((g[k>>2]=H,c[k>>2]|0)>>1),+g[k>>2]);E=F*(1.5-F*H*.5*F);F=I*E;I=J*E;E=d*+g[b+(a*60|0)+16>>2];g[v>>2]=w+E*(C*I-t*F-D);g[x>>2]=y+E*(C*F+t*I-z);z=+g[o>>2];t=+g[m>>2];g[j>>2]=+g[j>>2]+E*(z*I-t*F-p);g[A>>2]=+g[A>>2]+E*(z*F+t*I-q);q=+g[n>>2];t=+g[G>>2];g[l>>2]=+g[l>>2]+E*(q*I-t*F-r);g[B>>2]=+g[B>>2]+E*(q*F+t*I-u)}a=a+1|0;}while((a|0)<(e|0));return}function xl(a,b){a=a|0;b=b|0;var d=0.0,e=0,f=0,h=0,i=0,j=0,k=0,l=0,m=0,n=0.0,o=0.0,p=0.0,q=0.0,r=0,s=0.0,t=0,u=0.0,v=0,w=0.0;d=+g[b+4>>2]*+g[a+348>>2];e=c[a+252>>2]|0;if((e|0)<=0){return}f=a+96|0;h=a+104|0;i=b|0;b=c[a+248>>2]|0;a=0;do{if((c[b+(a*20|0)+8>>2]&8|0)!=0){j=c[b+(a*20|0)>>2]|0;k=c[b+(a*20|0)+4>>2]|0;l=c[f>>2]|0;m=l+(j<<3)|0;n=+g[m>>2];o=+g[m+4>>2];m=l+(k<<3)|0;p=+g[m>>2];l=c[h>>2]|0;q=+g[i>>2];r=l+(j<<3)|0;s=+g[r>>2];t=l+(j<<3)+4|0;u=+g[t>>2];j=l+(k<<3)|0;v=l+(k<<3)+4|0;w=p+q*+g[j>>2]-(n+q*s);n=+g[m+4>>2]+q*+g[v>>2]-(o+q*u);q=+R(w*w+n*n);o=(+g[b+(a*20|0)+16>>2]-q)*d*+g[b+(a*20|0)+12>>2]/q;q=w*o;w=n*o;g[r>>2]=s-q;g[t>>2]=u-w;g[j>>2]=q+ +g[j>>2];g[v>>2]=w+ +g[v>>2]}a=a+1|0;}while((a|0)<(e|0));return}



function yh(b,d){b=b|0;d=d|0;var e=0,f=0,h=0,i=0,j=0,l=0,m=0,n=0,o=0,p=0,q=0,r=0,s=0.0,t=0.0,u=0.0,v=0.0,w=0.0,x=0.0,y=0.0,z=0.0,A=0.0,B=0.0,C=0.0,D=0.0,E=0.0,F=0.0,G=0.0,H=0.0,I=0.0,J=0.0,K=0.0,L=0.0,M=0.0,N=0.0,O=0.0,P=0.0,Q=0.0,S=0.0,V=0.0,W=0.0,X=0.0;e=c[b+48>>2]|0;f=c[e+8>>2]|0;h=b+96|0;c[h>>2]=f;i=c[b+52>>2]|0;j=c[i+8>>2]|0;l=b+100|0;c[l>>2]=j;m=e+44|0;n=b+128|0;o=c[m>>2]|0;p=c[m+4>>2]|0;c[n>>2]=o;c[n+4>>2]=p;n=i+44|0;m=b+136|0;q=c[n>>2]|0;r=c[n+4>>2]|0;c[m>>2]=q;c[m+4>>2]=r;s=+g[e+136>>2];g[b+144>>2]=s;t=+g[i+136>>2];g[b+148>>2]=t;u=+g[e+144>>2];g[b+152>>2]=u;v=+g[i+144>>2];g[b+156>>2]=v;i=c[d+28>>2]|0;e=i+(f*12|0)|0;w=+g[e>>2];x=+g[e+4>>2];y=+g[i+(f*12|0)+8>>2];e=d+32|0;m=c[e>>2]|0;n=m+(f*12|0)|0;z=+g[n>>2];A=+g[n+4>>2];B=+g[m+(f*12|0)+8>>2];n=i+(j*12|0)|0;C=+g[n>>2];D=+g[n+4>>2];E=+g[i+(j*12|0)+8>>2];i=m+(j*12|0)|0;F=+g[i>>2];G=+g[i+4>>2];H=+g[m+(j*12|0)+8>>2];I=+U(y);J=+T(y);y=+U(E);K=+T(E);E=+g[b+68>>2]-(c[k>>2]=o,+g[k>>2]);L=+g[b+72>>2]-(c[k>>2]=p,+g[k>>2]);M=J*E-I*L;N=I*E+J*L;p=b+112|0;L=+M;J=+N;g[p>>2]=L;g[p+4>>2]=J;J=+g[b+76>>2]-(c[k>>2]=q,+g[k>>2]);L=+g[b+80>>2]-(c[k>>2]=r,+g[k>>2]);E=K*J-y*L;I=y*J+K*L;r=b+120|0;L=+E;K=+I;g[r>>2]=L;g[r+4>>2]=K;r=b+104|0;K=C+E-w-M;w=D+I-x-N;q=r;x=+K;D=+w;g[q>>2]=x;g[q+4>>2]=D;q=r|0;r=b+108|0;D=+R(K*K+w*w);g[b+88>>2]=D;c[b+164>>2]=D- +g[b+84>>2]>0.0?2:0;if(!(D>.004999999888241291)){g[q>>2]=0.0;g[r>>2]=0.0;g[b+160>>2]=0.0;g[b+92>>2]=0.0;return}x=1.0/D;D=x*K;g[q>>2]=D;K=x*w;g[r>>2]=K;w=M*K-N*D;x=K*E-D*I;C=t+(s+w*w*u)+x*x*v;if(C!=0.0){O=1.0/C}else{O=0.0}g[b+160>>2]=O;if((a[d+24|0]|0)==0){g[b+92>>2]=0.0;P=B;Q=H;S=F;V=G;W=z;X=A}else{r=b+92|0;O=+g[d+8>>2]*+g[r>>2];g[r>>2]=O;C=D*O;D=O*K;P=B-u*(D*M-C*N);Q=H+v*(D*E-C*I);S=F+C*t;V=G+D*t;W=z-C*s;X=A-D*s}r=(c[e>>2]|0)+(f*12|0)|0;s=+W;W=+X;g[r>>2]=s;g[r+4>>2]=W;r=c[e>>2]|0;g[r+((c[h>>2]|0)*12|0)+8>>2]=P;h=r+((c[l>>2]|0)*12|0)|0;P=+S;S=+V;g[h>>2]=P;g[h+4>>2]=S;g[(c[e>>2]|0)+((c[l>>2]|0)*12|0)+8>>2]=Q;return}function zh(a,b){a=a|0;b=b|0;var d=0,e=0,f=0,h=0,i=0,j=0.0,k=0.0,l=0.0,m=0,n=0,o=0.0,p=0.0,q=0.0,r=0.0,s=0.0,t=0.0,u=0.0,v=0.0,w=0.0,x=0.0,y=0.0,z=0.0;d=a+96|0;e=c[d>>2]|0;f=b+32|0;h=c[f>>2]|0;i=h+(e*12|0)|0;j=+g[i>>2];k=+g[i+4>>2];l=+g[h+(e*12|0)+8>>2];e=a+100|0;m=c[e>>2]|0;n=h+(m*12|0)|0;o=+g[n>>2];p=+g[n+4>>2];q=+g[h+(m*12|0)+8>>2];r=+g[a+116>>2];s=+g[a+112>>2];t=+g[a+124>>2];u=+g[a+120>>2];v=+g[a+88>>2]- +g[a+84>>2];w=+g[a+104>>2];x=+g[a+108>>2];y=(o+t*(-0.0-q)-(j+r*(-0.0-l)))*w+(p+q*u-(k+l*s))*x;if(v<0.0){z=y+v*+g[b+4>>2]}else{z=y}b=a+92|0;y=+g[b>>2];v=y+z*(-0.0- +g[a+160>>2]);z=v>0.0?0.0:v;g[b>>2]=z;v=z-y;y=w*v;w=x*v;v=+g[a+144>>2];x=l- +g[a+152>>2]*(s*w-r*y);r=+g[a+148>>2];s=q+ +g[a+156>>2]*(w*u-y*t);t=+(j-v*y);j=+(k-v*w);g[i>>2]=t;g[i+4>>2]=j;i=c[f>>2]|0;g[i+((c[d>>2]|0)*12|0)+8>>2]=x;d=i+((c[e>>2]|0)*12|0)|0;x=+(o+y*r);y=+(p+w*r);g[d>>2]=x;g[d+4>>2]=y;g[(c[f>>2]|0)+((c[e>>2]|0)*12|0)+8>>2]=s;return}function Ah(a,b){a=a|0;b=b|0;var d=0,e=0,f=0,h=0,i=0.0,j=0.0,k=0.0,l=0,m=0,n=0.0,o=0.0,p=0.0,q=0.0,r=0.0,s=0.0,t=0.0,u=0.0,v=0.0,w=0.0,x=0.0,y=0.0,z=0.0,A=0.0;d=a+96|0;e=c[d>>2]|0;f=b+28|0;b=c[f>>2]|0;h=b+(e*12|0)|0;i=+g[h>>2];j=+g[h+4>>2];k=+g[b+(e*12|0)+8>>2];e=a+100|0;l=c[e>>2]|0;m=b+(l*12|0)|0;n=+g[m>>2];o=+g[m+4>>2];p=+g[b+(l*12|0)+8>>2];q=+U(k);r=+T(k);s=+U(p);t=+T(p);u=+g[a+68>>2]- +g[a+128>>2];v=+g[a+72>>2]- +g[a+132>>2];w=r*u-q*v;x=q*u+r*v;v=+g[a+76>>2]- +g[a+136>>2];r=+g[a+80>>2]- +g[a+140>>2];u=t*v-s*r;q=s*v+t*r;r=n+u-i-w;t=o+q-j-x;v=+R(r*r+t*t);if(v<1.1920928955078125e-7){y=0.0;z=r;A=t}else{s=1.0/v;y=v;z=r*s;A=t*s}l=a+84|0;s=y- +g[l>>2];t=s<.20000000298023224?s:.20000000298023224;s=(t<0.0?0.0:t)*(-0.0- +g[a+160>>2]);t=z*s;z=A*s;s=+g[a+144>>2];A=k- +g[a+152>>2]*(w*z-x*t);x=+g[a+148>>2];w=p+ +g[a+156>>2]*(u*z-q*t);q=+(i-s*t);i=+(j-s*z);g[h>>2]=q;g[h+4>>2]=i;h=c[f>>2]|0;g[h+((c[d>>2]|0)*12|0)+8>>2]=A;d=h+((c[e>>2]|0)*12|0)|0;A=+(n+x*t);t=+(o+x*z);g[d>>2]=A;g[d+4>>2]=t;g[(c[f>>2]|0)+((c[e>>2]|0)*12|0)+8>>2]=w;return y- +g[l>>2]<.004999999888241291|0}function Bh(a,b){a=a|0;b=b|0;var d=0,e=0.0,f=0.0,h=0.0,i=0.0,j=0.0;d=c[b+48>>2]|0;e=+g[d+24>>2];f=+g[b+68>>2];h=+g[d+20>>2];i=+g[b+72>>2];j=f*h+e*i+ +g[d+16>>2];g[a>>2]=+g[d+12>>2]+(e*f-h*i);g[a+4>>2]=j;return}function Ch(a,b){a=a|0;b=b|0;var d=0,e=0.0,f=0.0,h=0.0,i=0.0,j=0.0;d=c[b+52>>2]|0;e=+g[d+24>>2];f=+g[b+76>>2];h=+g[d+20>>2];i=+g[b+80>>2];j=f*h+e*i+ +g[d+16>>2];g[a>>2]=+g[d+12>>2]+(e*f-h*i);g[a+4>>2]=j;return}function Dh(a,b,c){a=a|0;b=b|0;c=+c;var d=0.0;d=+g[b+92>>2]*c;c=d*+g[b+108>>2];g[a>>2]=+g[b+104>>2]*d;g[a+4>>2]=c;return}function Eh(a,b){a=a|0;b=+b;return+0.0}function Fh(a){a=a|0;var b=0,e=0,f=0,j=0,k=0.0;b=i;e=c[(c[a+48>>2]|0)+8>>2]|0;f=c[(c[a+52>>2]|0)+8>>2]|0;gn(3120,(j=i,i=i+1|0,i=i+7&-8,c[j>>2]=0,j)|0);i=j;gn(3592,(j=i,i=i+8|0,c[j>>2]=e,j)|0);i=j;gn(2816,(j=i,i=i+8|0,c[j>>2]=f,j)|0);i=j;gn(2392,(j=i,i=i+8|0,c[j>>2]=d[a+61|0]|0,j)|0);i=j;k=+g[a+72>>2];gn(1936,(j=i,i=i+16|0,h[j>>3]=+g[a+68>>2],h[j+8>>3]=k,j)|0);i=j;k=+g[a+80>>2];gn(1472,(j=i,i=i+16|0,h[j>>3]=+g[a+76>>2],h[j+8>>3]=k,j)|0);i=j;gn(1112,(j=i,i=i+8|0,h[j>>3]=+g[a+84>>2],j)|0);i=j;gn(736,(j=i,i=i+8|0,c[j>>2]=c[a+56>>2],j)|0);i=j;i=b;return}function Gh(a){a=a|0;return}function Hh(a){a=a|0;Jn(a);return}function Ih(a,b){a=a|0;b=b|0;var d=0,e=0,f=0,g=0,h=0,i=0,j=0,k=0,l=0,m=0,n=0,o=0,p=0,q=0;switch(c[a>>2]|0){case 8:{d=Vm(b,208)|0;if((d|0)==0){e=0}else{f=d;Jg(f,a);e=f}g=e|0;return g|0};case 9:{e=Vm(b,180)|0;if((e|0)==0){h=0}else{f=e;xg(f,a);h=f}g=h|0;return g|0};case 1:{h=Vm(b,228)|0;if((h|0)==0){i=0}else{f=h;Ph(f,a);i=f}g=i|0;return g|0};case 4:{i=Vm(b,196)|0;if((i|0)==0){j=0}else{f=i;si(f,a);j=f}g=j|0;return g|0};case 6:{j=Vm(b,276)|0;if((j|0)==0){k=0}else{f=j;Ug(f,a);k=f}g=k|0;return g|0};case 7:{k=Vm(b,224)|0;if((k|0)==0){l=0}else{f=k;fi(f,a);l=f}g=l|0;return g|0};case 3:{l=Vm(b,176)|0;if((l|0)==0){m=0}else{f=l;lg(f,a);m=f}g=m|0;return g|0};case 5:{m=Vm(b,168)|0;if((m|0)==0){n=0}else{f=m;Kf(f,a);n=f}g=n|0;return g|0};case 2:{n=Vm(b,256)|0;if((n|0)==0){o=0}else{f=n;fh(f,a);o=f}g=o|0;return g|0};case 10:{o=Vm(b,168)|0;if((o|0)==0){p=0}else{f=o;xh(f,a);p=f}g=p|0;return g|0};case 11:{p=Vm(b,192)|0;if((p|0)==0){q=0}else{b=p;Yf(b,a);q=b}g=q|0;return g|0};default:{g=0;return g|0}}return 0}function Jh(a,b){a=a|0;b=b|0;jb[c[(c[a>>2]|0)+24>>2]&255](a);switch(c[a+4>>2]|0){case 11:{Wm(b,a,192);return};case 10:{Wm(b,a,168);return};case 8:{Wm(b,a,208);return};case 9:{Wm(b,a,180);return};case 3:{Wm(b,a,176);return};case 4:{Wm(b,a,196);return};case 6:{Wm(b,a,276);return};case 1:{Wm(b,a,228);return};case 7:{Wm(b,a,224);return};case 5:{Wm(b,a,168);return};case 2:{Wm(b,a,256);return};default:{return}}}function Kh(b,d){b=b|0;d=d|0;c[b>>2]=4584;c[b+4>>2]=c[d>>2];c[b+8>>2]=0;c[b+12>>2]=0;c[b+48>>2]=c[d+8>>2];c[b+52>>2]=c[d+12>>2];c[b+56>>2]=0;a[b+61|0]=a[d+16|0]|0;a[b+60|0]=0;c[b+64>>2]=c[d+4>>2];Nn(b+16|0,0,32)|0;return}function Lh(a){a=a|0;var b=0;a=i;gn(3072,(b=i,i=i+1|0,i=i+7&-8,c[b>>2]=0,b)|0);i=b;i=a;return}function Mh(a){a=a|0;return}function Nh(a){a=a|0;Jn(a);return}function Oh(a,b,d,e){a=a|0;b=b|0;d=d|0;e=e|0;var f=0,h=0.0,i=0,j=0.0,k=0.0,l=0.0,m=0.0,n=0.0;c[a+8>>2]=b;c[a+12>>2]=d;f=e|0;h=+g[f>>2]- +g[b+12>>2];i=e+4|0;j=+g[i>>2]- +g[b+16>>2];k=+g[b+24>>2];l=+g[b+20>>2];e=a+20|0;m=+(h*k+j*l);n=+(k*j+h*(-0.0-l));g[e>>2]=m;g[e+4>>2]=n;n=+g[f>>2]- +g[d+12>>2];m=+g[i>>2]- +g[d+16>>2];l=+g[d+24>>2];h=+g[d+20>>2];i=a+28|0;j=+(n*l+m*h);k=+(l*m+n*(-0.0-h));g[i>>2]=j;g[i+4>>2]=k;g[a+36>>2]=+g[d+72>>2]- +g[b+72>>2];return}function Ph(b,d){b=b|0;d=d|0;var e=0,f=0,h=0;Kh(b|0,d|0);c[b>>2]=5056;e=d+20|0;f=b+68|0;h=c[e+4>>2]|0;c[f>>2]=c[e>>2];c[f+4>>2]=h;h=d+28|0;f=b+76|0;e=c[h+4>>2]|0;c[f>>2]=c[h>>2];c[f+4>>2]=e;g[b+116>>2]=+g[d+36>>2];Nn(b+84|0,0,16)|0;g[b+120>>2]=+g[d+44>>2];g[b+124>>2]=+g[d+48>>2];g[b+104>>2]=+g[d+60>>2];g[b+108>>2]=+g[d+56>>2];a[b+112|0]=a[d+40|0]|0;a[b+100|0]=a[d+52|0]|0;c[b+224>>2]=0;return}function Qh(b,d){b=b|0;d=d|0;var e=0,f=0,h=0,i=0,j=0,l=0,m=0,n=0,o=0,p=0,q=0,r=0,s=0.0,t=0.0,u=0.0,v=0.0,w=0.0,x=0.0,y=0.0,z=0.0,A=0.0,B=0.0,C=0.0,D=0.0,E=0.0,F=0.0,G=0.0,H=0.0,I=0.0,J=0.0,K=0.0,L=0.0,M=0.0,N=0.0,O=0.0,P=0.0,Q=0.0,R=0.0,S=0.0,V=0.0,W=0.0,X=0,Y=0,Z=0,_=0,$=0,aa=0,ba=0,ca=0,da=0,ea=0,fa=0,ga=0,ha=0,ia=0,ja=0,ka=0,la=0,ma=0,na=0,oa=0,pa=0,qa=0,ra=0,sa=0,ta=0,ua=0,va=0,wa=0,xa=0,ya=0,za=0,Aa=0,Ba=0,Ca=0,Da=0,Ea=0;e=c[b+48>>2]|0;f=c[e+8>>2]|0;h=b+128|0;c[h>>2]=f;i=c[b+52>>2]|0;j=c[i+8>>2]|0;l=b+132|0;c[l>>2]=j;m=e+44|0;n=b+152|0;o=c[m>>2]|0;p=c[m+4>>2]|0;c[n>>2]=o;c[n+4>>2]=p;n=i+44|0;m=b+160|0;q=c[n>>2]|0;r=c[n+4>>2]|0;c[m>>2]=q;c[m+4>>2]=r;s=+g[e+136>>2];g[b+168>>2]=s;t=+g[i+136>>2];g[b+172>>2]=t;u=+g[e+144>>2];g[b+176>>2]=u;v=+g[i+144>>2];g[b+180>>2]=v;i=c[d+28>>2]|0;w=+g[i+(f*12|0)+8>>2];e=d+32|0;m=c[e>>2]|0;n=m+(f*12|0)|0;x=+g[n>>2];y=+g[n+4>>2];z=+g[m+(f*12|0)+8>>2];A=+g[i+(j*12|0)+8>>2];i=m+(j*12|0)|0;B=+g[i>>2];C=+g[i+4>>2];D=+g[m+(j*12|0)+8>>2];E=+U(w);F=+T(w);G=+U(A);H=+T(A);I=+g[b+68>>2]-(c[k>>2]=o,+g[k>>2]);J=+g[b+72>>2]-(c[k>>2]=p,+g[k>>2]);K=F*I-E*J;L=E*I+F*J;p=b+136|0;J=+K;F=+L;g[p>>2]=J;g[p+4>>2]=F;F=+g[b+76>>2]-(c[k>>2]=q,+g[k>>2]);J=+g[b+80>>2]-(c[k>>2]=r,+g[k>>2]);I=H*F-G*J;E=G*F+H*J;r=b+144|0;J=+I;H=+E;g[r>>2]=J;g[r+4>>2]=H;H=u+v;r=H==0.0;J=s+t;g[b+184>>2]=J+u*L*L+v*E*E;F=-0.0-L;G=u*K*F-v*E*I;g[b+196>>2]=G;M=u*F-v*E;g[b+208>>2]=M;g[b+188>>2]=G;g[b+200>>2]=J+u*K*K+v*I*I;J=u*K+v*I;g[b+212>>2]=J;g[b+192>>2]=M;g[b+204>>2]=J;g[b+216>>2]=H;if(H>0.0){N=1.0/H}else{N=H}g[b+220>>2]=N;if((a[b+100|0]|0)==0|r){g[b+96>>2]=0.0}do{if((a[b+112|0]|0)==0|r){c[b+224>>2]=0}else{N=A-w- +g[b+116>>2];H=+g[b+124>>2];J=+g[b+120>>2];M=H-J;if(M>0.0){O=M}else{O=-0.0-M}if(O<.06981317698955536){c[b+224>>2]=3;break}if(!(N>J)){q=b+224|0;if((c[q>>2]|0)!=1){g[b+92>>2]=0.0}c[q>>2]=1;break}q=b+224|0;if(N<H){c[q>>2]=0;g[b+92>>2]=0.0;break}if((c[q>>2]|0)!=2){g[b+92>>2]=0.0}c[q>>2]=2}}while(0);r=b+84|0;if((a[d+24|0]|0)==0){Nn(r|0,0,16)|0;P=z;Q=D;R=B;S=C;V=x;W=y;X=c[e>>2]|0;Y=X+(f*12|0)|0;Z=Y;_=(g[k>>2]=V,c[k>>2]|0);$=(g[k>>2]=W,c[k>>2]|0);aa=$;ba=0;ca=0;da=aa;ea=_;fa=0;ga=ca|ea;ha=da|fa;ia=Z|0;c[ia>>2]=ga;ja=Z+4|0;c[ja>>2]=ha;ka=c[h>>2]|0;la=c[e>>2]|0;ma=la+(ka*12|0)+8|0;g[ma>>2]=P;na=c[l>>2]|0;oa=la+(na*12|0)|0;pa=oa;qa=(g[k>>2]=R,c[k>>2]|0);ra=(g[k>>2]=S,c[k>>2]|0);sa=ra;ta=0;ua=0;va=sa;wa=qa;xa=0;ya=ua|wa;za=va|xa;Aa=pa|0;c[Aa>>2]=ya;Ba=pa+4|0;c[Ba>>2]=za;Ca=c[l>>2]|0;Da=c[e>>2]|0;Ea=Da+(Ca*12|0)+8|0;g[Ea>>2]=Q;return}else{q=d+8|0;O=+g[q>>2];d=r|0;w=O*+g[d>>2];g[d>>2]=w;d=b+88|0;A=O*+g[d>>2];g[d>>2]=A;d=b+92|0;H=O*+g[d>>2];g[d>>2]=H;d=b+96|0;O=+g[q>>2]*+g[d>>2];g[d>>2]=O;P=z-u*(H+(O+(A*K-w*L)));Q=D+v*(H+(O+(A*I-w*E)));R=B+t*w;S=C+t*A;V=x-s*w;W=y-s*A;X=c[e>>2]|0;Y=X+(f*12|0)|0;Z=Y;_=(g[k>>2]=V,c[k>>2]|0);$=(g[k>>2]=W,c[k>>2]|0);aa=$;ba=0;ca=0;da=aa;ea=_;fa=0;ga=ca|ea;ha=da|fa;ia=Z|0;c[ia>>2]=ga;ja=Z+4|0;c[ja>>2]=ha;ka=c[h>>2]|0;la=c[e>>2]|0;ma=la+(ka*12|0)+8|0;g[ma>>2]=P;na=c[l>>2]|0;oa=la+(na*12|0)|0;pa=oa;qa=(g[k>>2]=R,c[k>>2]|0);ra=(g[k>>2]=S,c[k>>2]|0);sa=ra;ta=0;ua=0;va=sa;wa=qa;xa=0;ya=ua|wa;za=va|xa;Aa=pa|0;c[Aa>>2]=ya;Ba=pa+4|0;c[Ba>>2]=za;Ca=c[l>>2]|0;Da=c[e>>2]|0;Ea=Da+(Ca*12|0)+8|0;g[Ea>>2]=Q;return}}function Rh(b,d){b=b|0;d=d|0;var e=0,f=0,h=0,j=0,k=0,l=0,m=0,n=0,o=0,p=0,q=0,r=0,s=0,t=0,u=0.0,v=0.0,w=0.0,x=0,y=0.0,z=0.0,A=0.0,B=0.0,C=0.0,D=0.0,E=0.0,F=0.0,G=0.0,H=0.0,I=0.0,J=0.0,K=0.0,L=0.0,M=0,N=0,O=0,P=0,Q=0.0,R=0,S=0,T=0.0,U=0.0,V=0.0,W=0.0,X=0,Y=0.0,Z=0.0,_=0.0,$=0.0,aa=0.0,ba=0.0,ca=0.0;e=i;i=i+80|0;f=e|0;h=e+16|0;j=e+32|0;k=e+40|0;l=e+48|0;m=e+56|0;n=e+64|0;o=e+72|0;p=b+128|0;q=c[p>>2]|0;r=d+32|0;s=c[r>>2]|0;t=s+(q*12|0)|0;u=+g[t>>2];v=+g[t+4>>2];w=+g[s+(q*12|0)+8>>2];q=b+132|0;t=c[q>>2]|0;x=s+(t*12|0)|0;y=+g[x>>2];z=+g[x+4>>2];A=+g[s+(t*12|0)+8>>2];B=+g[b+168>>2];C=+g[b+172>>2];D=+g[b+176>>2];E=+g[b+180>>2];t=D+E==0.0;do{if((a[b+100|0]|0)==0){F=w;G=A}else{if((c[b+224>>2]|0)==3|t){F=w;G=A;break}s=b+96|0;H=+g[s>>2];I=+g[d>>2]*+g[b+104>>2];J=H+(A-w- +g[b+108>>2])*(-0.0- +g[b+220>>2]);K=-0.0-I;L=J<I?J:I;I=L<K?K:L;g[s>>2]=I;L=I-H;F=w-D*L;G=A+E*L}}while(0);do{if((a[b+112|0]|0)==0){M=15}else{d=b+224|0;if((c[d>>2]|0)==0|t){M=15;break}s=b+148|0;x=b+144|0;N=b+140|0;O=b+136|0;A=y+ +g[s>>2]*(-0.0-G)-u- +g[N>>2]*(-0.0-F);w=z+G*+g[x>>2]-v-F*+g[O>>2];g[f>>2]=A;g[f+4>>2]=w;g[f+8>>2]=G-F;P=b+184|0;Xm(h,P,f);L=+g[h>>2];H=-0.0-L;I=+g[h+4>>2];K=-0.0-I;J=+g[h+8>>2];Q=-0.0-J;R=c[d>>2]|0;do{if((R|0)==1){d=b+84|0;S=b+92|0;T=+g[S>>2];U=T-J;if(U<0.0){V=T*+g[b+212>>2]-w;g[j>>2]=T*+g[b+208>>2]-A;g[j+4>>2]=V;Ym(k,P,j);V=+g[k>>2];T=+g[k+4>>2];W=-0.0- +g[S>>2];X=d|0;g[X>>2]=V+ +g[X>>2];X=b+88|0;g[X>>2]=T+ +g[X>>2];g[S>>2]=0.0;Y=V;Z=T;_=W;break}else{X=d|0;g[X>>2]=+g[X>>2]-L;X=b+88|0;g[X>>2]=+g[X>>2]-I;g[S>>2]=U;Y=H;Z=K;_=Q;break}}else if((R|0)==2){S=b+84|0;X=b+92|0;U=+g[X>>2];W=U-J;if(W>0.0){T=U*+g[b+212>>2]-w;g[l>>2]=U*+g[b+208>>2]-A;g[l+4>>2]=T;Ym(m,P,l);T=+g[m>>2];U=+g[m+4>>2];V=-0.0- +g[X>>2];d=S|0;g[d>>2]=T+ +g[d>>2];d=b+88|0;g[d>>2]=U+ +g[d>>2];g[X>>2]=0.0;Y=T;Z=U;_=V;break}else{d=S|0;g[d>>2]=+g[d>>2]-L;d=b+88|0;g[d>>2]=+g[d>>2]-I;g[X>>2]=W;Y=H;Z=K;_=Q;break}}else if((R|0)==3){X=b+84|0;g[X>>2]=+g[X>>2]-L;X=b+88|0;g[X>>2]=+g[X>>2]-I;X=b+92|0;g[X>>2]=+g[X>>2]-J;Y=H;Z=K;_=Q}else{Y=H;Z=K;_=Q}}while(0);$=_+(Z*+g[O>>2]-Y*+g[N>>2]);aa=_+(Z*+g[x>>2]-Y*+g[s>>2]);ba=Y;ca=Z}}while(0);if((M|0)==15){M=b+148|0;m=b+144|0;l=b+140|0;k=b+136|0;Z=-0.0-(z+G*+g[m>>2]-v-F*+g[k>>2]);g[o>>2]=-0.0-(y+ +g[M>>2]*(-0.0-G)-u- +g[l>>2]*(-0.0-F));g[o+4>>2]=Z;Ym(n,b+184|0,o);Z=+g[n>>2];o=b+84|0;g[o>>2]=Z+ +g[o>>2];Y=+g[n+4>>2];n=b+88|0;g[n>>2]=Y+ +g[n>>2];$=Y*+g[k>>2]-Z*+g[l>>2];aa=Y*+g[m>>2]-Z*+g[M>>2];ba=Z;ca=Y}M=(c[r>>2]|0)+((c[p>>2]|0)*12|0)|0;Y=+(u-B*ba);u=+(v-B*ca);g[M>>2]=Y;g[M+4>>2]=u;M=c[r>>2]|0;g[M+((c[p>>2]|0)*12|0)+8>>2]=F-D*$;p=M+((c[q>>2]|0)*12|0)|0;$=+(y+C*ba);ba=+(z+C*ca);g[p>>2]=$;g[p+4>>2]=ba;g[(c[r>>2]|0)+((c[q>>2]|0)*12|0)+8>>2]=G+E*aa;i=e;return}function Sh(b,d){b=b|0;d=d|0;var e=0,f=0,h=0,i=0,j=0.0,k=0.0,l=0.0,m=0,n=0,o=0.0,p=0.0,q=0.0,r=0.0,s=0.0,t=0.0,u=0.0,v=0.0,w=0.0,x=0.0,y=0.0,z=0.0,A=0.0,B=0.0,C=0.0,D=0.0,E=0.0,F=0.0,G=0.0,H=0.0,I=0.0,J=0.0,K=0.0,L=0;e=b+128|0;f=c[e>>2]|0;h=d+28|0;d=c[h>>2]|0;i=d+(f*12|0)|0;j=+g[i>>2];k=+g[i+4>>2];l=+g[d+(f*12|0)+8>>2];f=b+132|0;m=c[f>>2]|0;n=d+(m*12|0)|0;o=+g[n>>2];p=+g[n+4>>2];q=+g[d+(m*12|0)+8>>2];m=b+176|0;d=b+180|0;do{if((a[b+112|0]|0)==0){r=l;s=q;t=0.0;u=+g[m>>2];v=+g[d>>2]}else{w=+g[d>>2];x=+g[m>>2];n=c[b+224>>2]|0;if((n|0)==0|w+x==0.0){r=l;s=q;t=0.0;u=x;v=w;break}y=q-l- +g[b+116>>2];do{if((n|0)==1){z=y- +g[b+120>>2];A=z+.03490658849477768;B=A<0.0?A:0.0;C=-0.0-z;D=(B<-.13962635397911072?-.13962635397911072:B)*(-0.0- +g[b+220>>2])}else if((n|0)==2){B=y- +g[b+124>>2];z=B+-.03490658849477768;A=z<.13962635397911072?z:.13962635397911072;C=B;D=(A<0.0?0.0:A)*(-0.0- +g[b+220>>2])}else if((n|0)==3){A=y- +g[b+120>>2];B=A<.13962635397911072?A:.13962635397911072;A=B<-.13962635397911072?-.13962635397911072:B;B=A*(-0.0- +g[b+220>>2]);if(A>0.0){C=A;D=B;break}C=-0.0-A;D=B}else{C=0.0;D=0.0}}while(0);r=l-D*x;s=q+D*w;t=C;u=x;v=w}}while(0);C=+U(r);D=+T(r);q=+U(s);l=+T(s);y=+g[b+68>>2]- +g[b+152>>2];B=+g[b+72>>2]- +g[b+156>>2];A=D*y-C*B;z=C*y+D*B;B=+g[b+76>>2]- +g[b+160>>2];D=+g[b+80>>2]- +g[b+164>>2];y=l*B-q*D;C=q*B+l*D;D=o+y-j-A;l=p+C-k-z;B=+R(D*D+l*l);q=+g[b+168>>2];E=+g[b+172>>2];F=q+E;G=F+z*z*u+C*C*v;H=y*v;I=z*A*(-0.0-u)-C*H;J=F+A*A*u+y*H;H=G*J-I*I;if(H!=0.0){K=1.0/H}else{K=H}H=-0.0-(D*J-l*I)*K;J=-0.0-(l*G-D*I)*K;K=+(j-q*H);j=+(k-q*J);g[i>>2]=K;g[i+4>>2]=j;i=c[h>>2]|0;g[i+((c[e>>2]|0)*12|0)+8>>2]=r-u*(A*J-z*H);e=i+((c[f>>2]|0)*12|0)|0;z=+(o+E*H);o=+(p+E*J);g[e>>2]=z;g[e+4>>2]=o;g[(c[h>>2]|0)+((c[f>>2]|0)*12|0)+8>>2]=s+v*(y*J-C*H);if(B>.004999999888241291){L=0;return L|0}L=t<=.03490658849477768;return L|0}function Th(a,b){a=a|0;b=b|0;var d=0,e=0.0,f=0.0,h=0.0,i=0.0,j=0.0;d=c[b+48>>2]|0;e=+g[d+24>>2];f=+g[b+68>>2];h=+g[d+20>>2];i=+g[b+72>>2];j=f*h+e*i+ +g[d+16>>2];g[a>>2]=+g[d+12>>2]+(e*f-h*i);g[a+4>>2]=j;return}function Uh(a,b){a=a|0;b=b|0;var d=0,e=0.0,f=0.0,h=0.0,i=0.0,j=0.0;d=c[b+52>>2]|0;e=+g[d+24>>2];f=+g[b+76>>2];h=+g[d+20>>2];i=+g[b+80>>2];j=f*h+e*i+ +g[d+16>>2];g[a>>2]=+g[d+12>>2]+(e*f-h*i);g[a+4>>2]=j;return}function Vh(a,b,c){a=a|0;b=b|0;c=+c;var d=0.0;d=+g[b+88>>2]*c;g[a>>2]=+g[b+84>>2]*c;g[a+4>>2]=d;return}function Wh(a,b){a=a|0;b=+b;return+(+g[a+92>>2]*b)}function Xh(a){a=a|0;return+(+g[(c[a+52>>2]|0)+72>>2]- +g[(c[a+48>>2]|0)+72>>2]- +g[a+116>>2])}function Yh(b){b=b|0;return(a[b+100|0]|0)!=0|0}function Zh(d,e){d=d|0;e=e|0;var f=0,h=0,i=0,j=0,k=0;f=c[d+48>>2]|0;h=f+4|0;i=b[h>>1]|0;if((i&2)==0){b[h>>1]=i|2;g[f+160>>2]=0.0}f=c[d+52>>2]|0;i=f+4|0;h=b[i>>1]|0;if(!((h&2)==0)){j=d+100|0;k=e&1;a[j]=k;return}b[i>>1]=h|2;g[f+160>>2]=0.0;j=d+100|0;k=e&1;a[j]=k;return}function _h(a,d){a=a|0;d=+d;var e=0,f=0,h=0,i=0;e=c[a+48>>2]|0;f=e+4|0;h=b[f>>1]|0;if((h&2)==0){b[f>>1]=h|2;g[e+160>>2]=0.0}e=c[a+52>>2]|0;h=e+4|0;f=b[h>>1]|0;if(!((f&2)==0)){i=a+108|0;g[i>>2]=d;return}b[h>>1]=f|2;g[e+160>>2]=0.0;i=a+108|0;g[i>>2]=d;return}function $h(b){b=b|0;return(a[b+112|0]|0)!=0|0}function ai(e,f){e=e|0;f=f|0;var h=0,i=0,j=0,k=0;h=e+112|0;if((f&1|0)==(d[h]|0|0)){return}i=c[e+48>>2]|0;j=i+4|0;k=b[j>>1]|0;if((k&2)==0){b[j>>1]=k|2;g[i+160>>2]=0.0}i=c[e+52>>2]|0;k=i+4|0;j=b[k>>1]|0;if((j&2)==0){b[k>>1]=j|2;g[i+160>>2]=0.0}a[h]=f&1;g[e+92>>2]=0.0;return}function bi(a){a=a|0;var b=0,e=0,f=0,j=0,k=0.0;b=i;e=c[(c[a+48>>2]|0)+8>>2]|0;f=c[(c[a+52>>2]|0)+8>>2]|0;gn(3040,(j=i,i=i+1|0,i=i+7&-8,c[j>>2]=0,j)|0);i=j;gn(3560,(j=i,i=i+8|0,c[j>>2]=e,j)|0);i=j;gn(2784,(j=i,i=i+8|0,c[j>>2]=f,j)|0);i=j;gn(2352,(j=i,i=i+8|0,c[j>>2]=d[a+61|0]|0,j)|0);i=j;k=+g[a+72>>2];gn(1888,(j=i,i=i+16|0,h[j>>3]=+g[a+68>>2],h[j+8>>3]=k,j)|0);i=j;k=+g[a+80>>2];gn(1424,(j=i,i=i+16|0,h[j>>3]=+g[a+76>>2],h[j+8>>3]=k,j)|0);i=j;gn(1080,(j=i,i=i+8|0,h[j>>3]=+g[a+116>>2],j)|0);i=j;gn(704,(j=i,i=i+8|0,c[j>>2]=d[a+112|0]|0,j)|0);i=j;gn(408,(j=i,i=i+8|0,h[j>>3]=+g[a+120>>2],j)|0);i=j;gn(128,(j=i,i=i+8|0,h[j>>3]=+g[a+124>>2],j)|0);i=j;gn(4040,(j=i,i=i+8|0,c[j>>2]=d[a+100|0]|0,j)|0);i=j;gn(3864,(j=i,i=i+8|0,h[j>>3]=+g[a+108>>2],j)|0);i=j;gn(3496,(j=i,i=i+8|0,h[j>>3]=+g[a+104>>2],j)|0);i=j;gn(3272,(j=i,i=i+8|0,c[j>>2]=c[a+56>>2],j)|0);i=j;i=b;return}function ci(a){a=a|0;return}function di(a){a=a|0;Jn(a);return}function ei(a,b,d,e,f){a=a|0;b=b|0;d=d|0;e=e|0;f=f|0;var h=0,i=0.0,j=0,k=0.0,l=0.0,m=0,n=0.0,o=0.0,p=0.0;c[a+8>>2]=b;c[a+12>>2]=d;h=e|0;i=+g[h>>2]- +g[b+12>>2];j=e+4|0;k=+g[j>>2]- +g[b+16>>2];e=b+24|0;l=+g[e>>2];m=b+20|0;n=+g[m>>2];b=a+20|0;o=+(i*l+k*n);p=+(l*k+i*(-0.0-n));g[b>>2]=o;g[b+4>>2]=p;p=+g[h>>2]- +g[d+12>>2];o=+g[j>>2]- +g[d+16>>2];n=+g[d+24>>2];i=+g[d+20>>2];d=a+28|0;k=+(p*n+o*i);l=+(n*o+p*(-0.0-i));g[d>>2]=k;g[d+4>>2]=l;l=+g[e>>2];k=+g[f>>2];i=+g[m>>2];p=+g[f+4>>2];f=a+36|0;o=+(l*k+i*p);n=+(k*(-0.0-i)+l*p);g[f>>2]=o;g[f+4>>2]=n;return}function fi(b,d){b=b|0;d=d|0;var e=0,f=0,h=0,i=0;Kh(b|0,d|0);c[b>>2]=5536;e=d+20|0;f=b+76|0;h=c[e+4>>2]|0;c[f>>2]=c[e>>2];c[f+4>>2]=h;h=d+28|0;f=b+84|0;e=c[h+4>>2]|0;c[f>>2]=c[h>>2];c[f+4>>2]=e;e=d+36|0;f=b+92|0;h=c[e>>2]|0;i=c[e+4>>2]|0;c[f>>2]=h;c[f+4>>2]=i;f=b+100|0;g[f>>2]=(c[k>>2]=i,+g[k>>2])*-1.0;c[f+4>>2]=h;g[b+204>>2]=0.0;g[b+108>>2]=0.0;g[b+208>>2]=0.0;g[b+112>>2]=0.0;g[b+212>>2]=0.0;g[b+116>>2]=0.0;g[b+120>>2]=+g[d+48>>2];g[b+124>>2]=+g[d+52>>2];a[b+128|0]=a[d+44|0]|0;g[b+68>>2]=+g[d+56>>2];g[b+72>>2]=+g[d+60>>2];g[b+216>>2]=0.0;g[b+220>>2]=0.0;Nn(b+172|0,0,16)|0;return}function gi(b,d){b=b|0;d=d|0;var e=0,f=0,h=0,i=0,j=0,l=0,m=0,n=0,o=0,p=0,q=0,r=0,s=0.0,t=0.0,u=0.0,v=0.0,w=0.0,x=0.0,y=0.0,z=0.0,A=0.0,B=0.0,C=0.0,D=0.0,E=0.0,F=0.0,G=0.0,H=0.0,I=0.0,J=0.0,K=0.0,L=0.0,M=0.0,N=0.0,O=0.0,P=0.0,Q=0.0,R=0.0,S=0.0,V=0.0,W=0.0,X=0.0,Y=0.0,Z=0.0,_=0.0,$=0.0,aa=0.0,ba=0.0,ca=0.0,da=0.0,ea=0.0,fa=0,ga=0,ha=0,ia=0,ja=0,ka=0,la=0,ma=0,na=0,oa=0,pa=0,qa=0,ra=0,sa=0,ta=0,ua=0,va=0,wa=0,xa=0,ya=0,za=0,Aa=0,Ba=0,Ca=0,Da=0,Ea=0,Fa=0,Ga=0,Ha=0,Ia=0,Ja=0,Ka=0,La=0,Ma=0,Na=0,Oa=0;e=c[b+48>>2]|0;f=c[e+8>>2]|0;h=b+132|0;c[h>>2]=f;i=c[b+52>>2]|0;j=c[i+8>>2]|0;l=b+136|0;c[l>>2]=j;m=e+44|0;n=b+140|0;o=c[m>>2]|0;p=c[m+4>>2]|0;c[n>>2]=o;c[n+4>>2]=p;n=i+44|0;m=b+148|0;q=c[n>>2]|0;r=c[n+4>>2]|0;c[m>>2]=q;c[m+4>>2]=r;s=+g[e+136>>2];g[b+156>>2]=s;t=+g[i+136>>2];g[b+160>>2]=t;u=+g[e+144>>2];g[b+164>>2]=u;v=+g[i+144>>2];g[b+168>>2]=v;i=c[d+28>>2]|0;e=i+(f*12|0)|0;w=+g[e>>2];x=+g[e+4>>2];y=+g[i+(f*12|0)+8>>2];e=d+32|0;m=c[e>>2]|0;n=m+(f*12|0)|0;z=+g[n>>2];A=+g[n+4>>2];B=+g[m+(f*12|0)+8>>2];n=i+(j*12|0)|0;C=+g[n>>2];D=+g[n+4>>2];E=+g[i+(j*12|0)+8>>2];i=m+(j*12|0)|0;F=+g[i>>2];G=+g[i+4>>2];H=+g[m+(j*12|0)+8>>2];I=+U(y);J=+T(y);y=+U(E);K=+T(E);E=+g[b+76>>2]-(c[k>>2]=o,+g[k>>2]);L=+g[b+80>>2]-(c[k>>2]=p,+g[k>>2]);M=J*E-I*L;N=I*E+J*L;L=+g[b+84>>2]-(c[k>>2]=q,+g[k>>2]);E=+g[b+88>>2]-(c[k>>2]=r,+g[k>>2]);O=K*L-y*E;P=y*L+K*E;E=C+O-w-M;w=D+P-x-N;x=+g[b+100>>2];D=+g[b+104>>2];C=J*x-I*D;K=I*x+J*D;r=b+180|0;D=+C;x=+K;g[r>>2]=D;g[r+4>>2]=x;x=M+E;M=N+w;N=K*x-C*M;g[b+196>>2]=N;D=O*K-P*C;g[b+200>>2]=D;L=s+t;y=L+N*u*N+D*v*D;if(y>0.0){Q=1.0/y}else{Q=y}g[b+204>>2]=Q;r=b+212|0;g[r>>2]=0.0;q=b+216|0;g[q>>2]=0.0;p=b+220|0;g[p>>2]=0.0;Q=+g[b+68>>2];do{if(Q>0.0){y=+g[b+92>>2];R=+g[b+96>>2];S=J*y-I*R;V=I*y+J*R;o=b+172|0;R=+S;y=+V;g[o>>2]=R;g[o+4>>2]=y;y=x*V-M*S;g[b+188>>2]=y;R=O*V-P*S;g[b+192>>2]=R;W=L+y*u*y+R*v*R;if(!(W>0.0)){break}R=1.0/W;g[r>>2]=R;y=Q*6.2831854820251465;X=y*R*y;Y=+g[d>>2];Z=Y*(y*R*2.0*+g[b+72>>2]+Y*X);if(Z>0.0){_=1.0/Z}else{_=Z}g[p>>2]=_;g[q>>2]=(E*S+w*V)*Y*X*_;X=W+_;g[r>>2]=X;if(!(X>0.0)){break}g[r>>2]=1.0/X}else{g[b+116>>2]=0.0}}while(0);do{if((a[b+128|0]|0)==0){g[b+208>>2]=0.0;g[b+112>>2]=0.0}else{_=v+u;r=b+208|0;g[r>>2]=_;if(!(_>0.0)){break}g[r>>2]=1.0/_}}while(0);if((a[d+24|0]|0)==0){g[b+108>>2]=0.0;g[b+116>>2]=0.0;g[b+112>>2]=0.0;$=B;aa=H;ba=F;ca=G;da=z;ea=A;fa=c[e>>2]|0;ga=fa+(f*12|0)|0;ha=ga;ia=(g[k>>2]=da,c[k>>2]|0);ja=(g[k>>2]=ea,c[k>>2]|0);ka=ja;la=0;ma=0;na=ka;oa=ia;pa=0;qa=ma|oa;ra=na|pa;sa=ha|0;c[sa>>2]=qa;ta=ha+4|0;c[ta>>2]=ra;ua=c[h>>2]|0;va=c[e>>2]|0;wa=va+(ua*12|0)+8|0;g[wa>>2]=$;xa=c[l>>2]|0;ya=va+(xa*12|0)|0;za=ya;Aa=(g[k>>2]=ba,c[k>>2]|0);Ba=(g[k>>2]=ca,c[k>>2]|0);Ca=Ba;Da=0;Ea=0;Fa=Ca;Ga=Aa;Ha=0;Ia=Ea|Ga;Ja=Fa|Ha;Ka=za|0;c[Ka>>2]=Ia;La=za+4|0;c[La>>2]=Ja;Ma=c[l>>2]|0;Na=c[e>>2]|0;Oa=Na+(Ma*12|0)+8|0;g[Oa>>2]=aa;return}else{r=d+8|0;d=b+108|0;_=+g[r>>2]*+g[d>>2];g[d>>2]=_;d=b+116|0;w=+g[r>>2]*+g[d>>2];g[d>>2]=w;d=b+112|0;E=+g[r>>2]*+g[d>>2];g[d>>2]=E;Q=_*C+w*+g[b+172>>2];C=_*K+w*+g[b+176>>2];$=B-(E+(_*N+w*+g[b+188>>2]))*u;aa=H+(E+(_*D+w*+g[b+192>>2]))*v;ba=F+Q*t;ca=G+C*t;da=z-Q*s;ea=A-C*s;fa=c[e>>2]|0;ga=fa+(f*12|0)|0;ha=ga;ia=(g[k>>2]=da,c[k>>2]|0);ja=(g[k>>2]=ea,c[k>>2]|0);ka=ja;la=0;ma=0;na=ka;oa=ia;pa=0;qa=ma|oa;ra=na|pa;sa=ha|0;c[sa>>2]=qa;ta=ha+4|0;c[ta>>2]=ra;ua=c[h>>2]|0;va=c[e>>2]|0;wa=va+(ua*12|0)+8|0;g[wa>>2]=$;xa=c[l>>2]|0;ya=va+(xa*12|0)|0;za=ya;Aa=(g[k>>2]=ba,c[k>>2]|0);Ba=(g[k>>2]=ca,c[k>>2]|0);Ca=Ba;Da=0;Ea=0;Fa=Ca;Ga=Aa;Ha=0;Ia=Ea|Ga;Ja=Fa|Ha;Ka=za|0;c[Ka>>2]=Ia;La=za+4|0;c[La>>2]=Ja;Ma=c[l>>2]|0;Na=c[e>>2]|0;Oa=Na+(Ma*12|0)+8|0;g[Oa>>2]=aa;return}}function hi(a,b){a=a|0;b=b|0;var d=0.0,e=0.0,f=0.0,h=0.0,i=0,j=0,k=0,l=0,m=0,n=0.0,o=0.0,p=0.0,q=0,r=0,s=0.0,t=0.0,u=0.0,v=0.0,w=0.0,x=0.0,y=0.0,z=0.0,A=0.0;d=+g[a+156>>2];e=+g[a+160>>2];f=+g[a+164>>2];h=+g[a+168>>2];i=a+132|0;j=c[i>>2]|0;k=b+32|0;l=c[k>>2]|0;m=l+(j*12|0)|0;n=+g[m>>2];o=+g[m+4>>2];p=+g[l+(j*12|0)+8>>2];j=a+136|0;q=c[j>>2]|0;r=l+(q*12|0)|0;s=+g[r>>2];t=+g[r+4>>2];u=+g[l+(q*12|0)+8>>2];v=+g[a+172>>2];w=+g[a+176>>2];x=+g[a+192>>2];y=+g[a+188>>2];q=a+116|0;z=+g[q>>2];A=(+g[a+216>>2]+(u*x+(v*(s-n)+w*(t-o))-p*y)+ +g[a+220>>2]*z)*(-0.0- +g[a+212>>2]);g[q>>2]=z+A;z=v*A;v=w*A;w=n-d*z;n=o-d*v;o=p-f*A*y;y=s+e*z;z=t+e*v;v=u+h*A*x;q=a+112|0;x=+g[q>>2];A=+g[b>>2]*+g[a+120>>2];u=x+(v-o- +g[a+124>>2])*(-0.0- +g[a+208>>2]);t=-0.0-A;s=u<A?u:A;A=s<t?t:s;g[q>>2]=A;s=A-x;x=o-f*s;o=v+h*s;s=+g[a+180>>2];v=+g[a+184>>2];A=+g[a+200>>2];t=+g[a+196>>2];u=((y-w)*s+(z-n)*v+A*o-t*x)*(-0.0- +g[a+204>>2]);q=a+108|0;g[q>>2]=+g[q>>2]+u;p=s*u;s=v*u;v=+(w-d*p);w=+(n-d*s);g[m>>2]=v;g[m+4>>2]=w;m=c[k>>2]|0;g[m+((c[i>>2]|0)*12|0)+8>>2]=x-f*t*u;i=m+((c[j>>2]|0)*12|0)|0;t=+(y+e*p);p=+(z+e*s);g[i>>2]=t;g[i+4>>2]=p;g[(c[k>>2]|0)+((c[j>>2]|0)*12|0)+8>>2]=o+h*A*u;return}function ii(a,b){a=a|0;b=b|0;var d=0,e=0,f=0,h=0,i=0.0,j=0.0,k=0.0,l=0,m=0,n=0.0,o=0.0,p=0.0,q=0.0,r=0.0,s=0.0,t=0.0,u=0.0,v=0.0,w=0.0,x=0.0,y=0.0,z=0.0,A=0.0,B=0.0,C=0.0,D=0.0,E=0.0,F=0.0,G=0.0,H=0.0,I=0;d=a+132|0;e=c[d>>2]|0;f=b+28|0;b=c[f>>2]|0;h=b+(e*12|0)|0;i=+g[h>>2];j=+g[h+4>>2];k=+g[b+(e*12|0)+8>>2];e=a+136|0;l=c[e>>2]|0;m=b+(l*12|0)|0;n=+g[m>>2];o=+g[m+4>>2];p=+g[b+(l*12|0)+8>>2];q=+U(k);r=+T(k);s=+U(p);t=+T(p);u=+g[a+76>>2]- +g[a+140>>2];v=+g[a+80>>2]- +g[a+144>>2];w=r*u-q*v;x=q*u+r*v;v=+g[a+84>>2]- +g[a+148>>2];u=+g[a+88>>2]- +g[a+152>>2];y=t*v-s*u;z=s*v+t*u;u=n-i+y-w;t=o-j+z-x;v=+g[a+100>>2];s=+g[a+104>>2];A=r*v-q*s;B=q*v+r*s;s=A*u+B*t;r=+g[a+156>>2];v=+g[a+160>>2];q=+g[a+164>>2];C=+g[a+196>>2];D=+g[a+168>>2];E=+g[a+200>>2];F=r+v+C*q*C+E*D*E;if(F!=0.0){G=(-0.0-s)/F}else{G=0.0}F=A*G;E=B*G;C=+(i-F*r);i=+(j-E*r);g[h>>2]=C;g[h+4>>2]=i;h=c[f>>2]|0;g[h+((c[d>>2]|0)*12|0)+8>>2]=k-(B*(w+u)-A*(x+t))*G*q;d=h+((c[e>>2]|0)*12|0)|0;q=+(n+F*v);F=+(o+E*v);g[d>>2]=q;g[d+4>>2]=F;g[(c[f>>2]|0)+((c[e>>2]|0)*12|0)+8>>2]=p+(y*B-z*A)*G*D;if(s>0.0){H=s;I=H<=.004999999888241291;return I|0}H=-0.0-s;I=H<=.004999999888241291;return I|0}function ji(a,b){a=a|0;b=b|0;var d=0,e=0.0,f=0.0,h=0.0,i=0.0,j=0.0;d=c[b+48>>2]|0;e=+g[d+24>>2];f=+g[b+76>>2];h=+g[d+20>>2];i=+g[b+80>>2];j=f*h+e*i+ +g[d+16>>2];g[a>>2]=+g[d+12>>2]+(e*f-h*i);g[a+4>>2]=j;return}function ki(a,b){a=a|0;b=b|0;var d=0,e=0.0,f=0.0,h=0.0,i=0.0,j=0.0;d=c[b+52>>2]|0;e=+g[d+24>>2];f=+g[b+84>>2];h=+g[d+20>>2];i=+g[b+88>>2];j=f*h+e*i+ +g[d+16>>2];g[a>>2]=+g[d+12>>2]+(e*f-h*i);g[a+4>>2]=j;return}function li(a,b,c){a=a|0;b=b|0;c=+c;var d=0.0,e=0.0,f=0.0;d=+g[b+108>>2];e=+g[b+116>>2];f=(d*+g[b+184>>2]+e*+g[b+176>>2])*c;g[a>>2]=(d*+g[b+180>>2]+e*+g[b+172>>2])*c;g[a+4>>2]=f;return}function mi(a,b){a=a|0;b=+b;return+(+g[a+112>>2]*b)}function ni(a,d){a=a|0;d=+d;var e=0,f=0,h=0,i=0;e=c[a+48>>2]|0;f=e+4|0;h=b[f>>1]|0;if((h&2)==0){b[f>>1]=h|2;g[e+160>>2]=0.0}e=c[a+52>>2]|0;h=e+4|0;f=b[h>>1]|0;if(!((f&2)==0)){i=a+124|0;g[i>>2]=d;return}b[h>>1]=f|2;g[e+160>>2]=0.0;i=a+124|0;g[i>>2]=d;return}function oi(a){a=a|0;var b=0,e=0,f=0,j=0,k=0.0;b=i;e=c[(c[a+48>>2]|0)+8>>2]|0;f=c[(c[a+52>>2]|0)+8>>2]|0;gn(2696,(j=i,i=i+1|0,i=i+7&-8,c[j>>2]=0,j)|0);i=j;gn(3416,(j=i,i=i+8|0,c[j>>2]=e,j)|0);i=j;gn(2752,(j=i,i=i+8|0,c[j>>2]=f,j)|0);i=j;gn(2312,(j=i,i=i+8|0,c[j>>2]=d[a+61|0]|0,j)|0);i=j;k=+g[a+80>>2];gn(1840,(j=i,i=i+16|0,h[j>>3]=+g[a+76>>2],h[j+8>>3]=k,j)|0);i=j;k=+g[a+88>>2];gn(1376,(j=i,i=i+16|0,h[j>>3]=+g[a+84>>2],h[j+8>>3]=k,j)|0);i=j;k=+g[a+96>>2];gn(1040,(j=i,i=i+16|0,h[j>>3]=+g[a+92>>2],h[j+8>>3]=k,j)|0);i=j;gn(672,(j=i,i=i+8|0,c[j>>2]=d[a+128|0]|0,j)|0);i=j;gn(376,(j=i,i=i+8|0,h[j>>3]=+g[a+124>>2],j)|0);i=j;gn(96,(j=i,i=i+8|0,h[j>>3]=+g[a+120>>2],j)|0);i=j;gn(4008,(j=i,i=i+8|0,h[j>>3]=+g[a+68>>2],j)|0);i=j;gn(3832,(j=i,i=i+8|0,h[j>>3]=+g[a+72>>2],j)|0);i=j;gn(3448,(j=i,i=i+8|0,c[j>>2]=c[a+56>>2],j)|0);i=j;i=b;return}function pi(a){a=a|0;return}function qi(a){a=a|0;Jn(a);return}function ri(a,b,d,e,f,h,i,j){a=a|0;b=b|0;d=d|0;e=e|0;f=f|0;h=h|0;i=i|0;j=+j;var k=0,l=0,m=0,n=0.0,o=0.0,p=0.0,q=0.0,r=0.0,s=0.0;c[a+8>>2]=b;c[a+12>>2]=d;k=e;l=a+20|0;m=c[k+4>>2]|0;c[l>>2]=c[k>>2];c[l+4>>2]=m;m=f;l=a+28|0;k=c[m+4>>2]|0;c[l>>2]=c[m>>2];c[l+4>>2]=k;k=h|0;n=+g[k>>2]- +g[b+12>>2];l=h+4|0;o=+g[l>>2]- +g[b+16>>2];p=+g[b+24>>2];q=+g[b+20>>2];b=a+36|0;r=+(n*p+o*q);s=+(p*o+n*(-0.0-q));g[b>>2]=r;g[b+4>>2]=s;b=i|0;s=+g[b>>2]- +g[d+12>>2];h=i+4|0;r=+g[h>>2]- +g[d+16>>2];q=+g[d+24>>2];n=+g[d+20>>2];d=a+44|0;o=+(s*q+r*n);p=+(q*r+s*(-0.0-n));g[d>>2]=o;g[d+4>>2]=p;p=+g[k>>2]- +g[e>>2];o=+g[l>>2]- +g[e+4>>2];g[a+52>>2]=+R(p*p+o*o);o=+g[b>>2]- +g[f>>2];p=+g[h>>2]- +g[f+4>>2];g[a+56>>2]=+R(o*o+p*p);g[a+60>>2]=j;return}function si(a,b){a=a|0;b=b|0;var d=0,e=0,f=0,h=0.0;Kh(a|0,b|0);c[a>>2]=5416;d=b+20|0;e=a+68|0;f=c[d+4>>2]|0;c[e>>2]=c[d>>2];c[e+4>>2]=f;f=b+28|0;e=a+76|0;d=c[f+4>>2]|0;c[e>>2]=c[f>>2];c[e+4>>2]=d;d=b+36|0;e=a+92|0;f=c[d+4>>2]|0;c[e>>2]=c[d>>2];c[e+4>>2]=f;f=b+44|0;e=a+100|0;d=c[f+4>>2]|0;c[e>>2]=c[f>>2];c[e+4>>2]=d;d=b+52|0;g[a+84>>2]=+g[d>>2];e=b+56|0;g[a+88>>2]=+g[e>>2];h=+g[b+60>>2];g[a+112>>2]=h;g[a+108>>2]=+g[d>>2]+h*+g[e>>2];g[a+116>>2]=0.0;return}function ti(b,d){b=b|0;d=d|0;var e=0,f=0,h=0,i=0,j=0,l=0,m=0,n=0,o=0,p=0,q=0,r=0,s=0.0,t=0.0,u=0.0,v=0.0,w=0.0,x=0.0,y=0.0,z=0.0,A=0.0,B=0.0,C=0.0,D=0.0,E=0.0,F=0.0,G=0.0,H=0.0,I=0.0,J=0.0,K=0.0,L=0.0,M=0.0,N=0.0,O=0.0,P=0.0,Q=0.0,S=0.0,V=0.0,W=0.0,X=0.0,Y=0.0,Z=0.0,_=0.0,$=0.0,aa=0,ba=0,ca=0,da=0,ea=0,fa=0,ga=0,ha=0,ia=0,ja=0,ka=0,la=0,ma=0,na=0,oa=0,pa=0,qa=0,ra=0,sa=0,ta=0,ua=0,va=0,wa=0,xa=0,ya=0,za=0,Aa=0,Ba=0,Ca=0,Da=0,Ea=0,Fa=0,Ga=0,Ha=0,Ia=0,Ja=0;e=c[b+48>>2]|0;f=c[e+8>>2]|0;h=b+120|0;c[h>>2]=f;i=c[b+52>>2]|0;j=c[i+8>>2]|0;l=b+124|0;c[l>>2]=j;m=e+44|0;n=b+160|0;o=c[m>>2]|0;p=c[m+4>>2]|0;c[n>>2]=o;c[n+4>>2]=p;n=i+44|0;m=b+168|0;q=c[n>>2]|0;r=c[n+4>>2]|0;c[m>>2]=q;c[m+4>>2]=r;s=+g[e+136>>2];g[b+176>>2]=s;t=+g[i+136>>2];g[b+180>>2]=t;u=+g[e+144>>2];g[b+184>>2]=u;v=+g[i+144>>2];g[b+188>>2]=v;i=c[d+28>>2]|0;e=i+(f*12|0)|0;w=+g[e>>2];x=+g[e+4>>2];y=+g[i+(f*12|0)+8>>2];e=d+32|0;m=c[e>>2]|0;n=m+(f*12|0)|0;z=+g[n>>2];A=+g[n+4>>2];B=+g[m+(f*12|0)+8>>2];n=i+(j*12|0)|0;C=+g[n>>2];D=+g[n+4>>2];E=+g[i+(j*12|0)+8>>2];i=m+(j*12|0)|0;F=+g[i>>2];G=+g[i+4>>2];H=+g[m+(j*12|0)+8>>2];I=+U(y);J=+T(y);y=+U(E);K=+T(E);E=+g[b+92>>2]-(c[k>>2]=o,+g[k>>2]);L=+g[b+96>>2]-(c[k>>2]=p,+g[k>>2]);M=J*E-I*L;N=I*E+J*L;p=b+144|0;L=+M;J=+N;g[p>>2]=L;g[p+4>>2]=J;J=+g[b+100>>2]-(c[k>>2]=q,+g[k>>2]);L=+g[b+104>>2]-(c[k>>2]=r,+g[k>>2]);E=K*J-y*L;I=y*J+K*L;r=b+152|0;L=+E;K=+I;g[r>>2]=L;g[r+4>>2]=K;r=b+128|0;K=w+M- +g[b+68>>2];w=x+N- +g[b+72>>2];q=r;x=+K;L=+w;g[q>>2]=x;g[q+4>>2]=L;q=b+136|0;L=C+E- +g[b+76>>2];C=D+I- +g[b+80>>2];p=q;D=+L;x=+C;g[p>>2]=D;g[p+4>>2]=x;p=r|0;x=+R(K*K+w*w);r=q|0;D=+R(L*L+C*C);if(x>.04999999701976776){J=1.0/x;x=K*J;g[p>>2]=x;O=J*w;P=x}else{g[p>>2]=0.0;O=0.0;P=0.0}g[b+132>>2]=O;if(D>.04999999701976776){x=1.0/D;D=x*L;g[r>>2]=D;Q=x*C;S=D}else{g[r>>2]=0.0;Q=0.0;S=0.0}g[b+140>>2]=Q;D=M*O-N*P;C=E*Q-I*S;x=+g[b+112>>2];L=s+D*D*u+x*x*(t+C*C*v);if(L>0.0){V=1.0/L}else{V=L}g[b+192>>2]=V;if((a[d+24|0]|0)==0){g[b+116>>2]=0.0;W=B;X=H;Y=F;Z=G;_=z;$=A;aa=c[e>>2]|0;ba=aa+(f*12|0)|0;ca=ba;da=(g[k>>2]=_,c[k>>2]|0);ea=(g[k>>2]=$,c[k>>2]|0);fa=ea;ga=0;ha=0;ia=fa;ja=da;ka=0;la=ha|ja;ma=ia|ka;na=ca|0;c[na>>2]=la;oa=ca+4|0;c[oa>>2]=ma;pa=c[h>>2]|0;qa=c[e>>2]|0;ra=qa+(pa*12|0)+8|0;g[ra>>2]=W;sa=c[l>>2]|0;ta=qa+(sa*12|0)|0;ua=ta;va=(g[k>>2]=Y,c[k>>2]|0);wa=(g[k>>2]=Z,c[k>>2]|0);xa=wa;ya=0;za=0;Aa=xa;Ba=va;Ca=0;Da=za|Ba;Ea=Aa|Ca;Fa=ua|0;c[Fa>>2]=Da;Ga=ua+4|0;c[Ga>>2]=Ea;Ha=c[l>>2]|0;Ia=c[e>>2]|0;Ja=Ia+(Ha*12|0)+8|0;g[Ja>>2]=X;return}else{r=b+116|0;V=+g[d+8>>2]*+g[r>>2];g[r>>2]=V;L=-0.0-V;C=P*L;P=O*L;L=V*(-0.0-x);x=S*L;S=Q*L;W=B+u*(P*M-C*N);X=H+v*(S*E-x*I);Y=F+x*t;Z=G+S*t;_=z+C*s;$=A+P*s;aa=c[e>>2]|0;ba=aa+(f*12|0)|0;ca=ba;da=(g[k>>2]=_,c[k>>2]|0);ea=(g[k>>2]=$,c[k>>2]|0);fa=ea;ga=0;ha=0;ia=fa;ja=da;ka=0;la=ha|ja;ma=ia|ka;na=ca|0;c[na>>2]=la;oa=ca+4|0;c[oa>>2]=ma;pa=c[h>>2]|0;qa=c[e>>2]|0;ra=qa+(pa*12|0)+8|0;g[ra>>2]=W;sa=c[l>>2]|0;ta=qa+(sa*12|0)|0;ua=ta;va=(g[k>>2]=Y,c[k>>2]|0);wa=(g[k>>2]=Z,c[k>>2]|0);xa=wa;ya=0;za=0;Aa=xa;Ba=va;Ca=0;Da=za|Ba;Ea=Aa|Ca;Fa=ua|0;c[Fa>>2]=Da;Ga=ua+4|0;c[Ga>>2]=Ea;Ha=c[l>>2]|0;Ia=c[e>>2]|0;Ja=Ia+(Ha*12|0)+8|0;g[Ja>>2]=X;return}}function ui(a,b){a=a|0;b=b|0;var d=0,e=0,f=0,h=0,i=0.0,j=0.0,k=0.0,l=0,m=0,n=0.0,o=0.0,p=0.0,q=0.0,r=0.0,s=0.0,t=0.0,u=0.0,v=0.0,w=0.0,x=0.0,y=0.0,z=0.0,A=0.0,B=0.0;d=a+120|0;e=c[d>>2]|0;f=b+32|0;b=c[f>>2]|0;h=b+(e*12|0)|0;i=+g[h>>2];j=+g[h+4>>2];k=+g[b+(e*12|0)+8>>2];e=a+124|0;l=c[e>>2]|0;m=b+(l*12|0)|0;n=+g[m>>2];o=+g[m+4>>2];p=+g[b+(l*12|0)+8>>2];q=+g[a+148>>2];r=+g[a+144>>2];s=+g[a+156>>2];t=+g[a+152>>2];u=+g[a+128>>2];v=+g[a+132>>2];w=+g[a+112>>2];x=+g[a+136>>2];y=+g[a+140>>2];z=(-0.0-((i+q*(-0.0-k))*u+(j+k*r)*v)-w*((n+s*(-0.0-p))*x+(o+p*t)*y))*(-0.0- +g[a+192>>2]);l=a+116|0;g[l>>2]=+g[l>>2]+z;A=-0.0-z;B=u*A;u=v*A;A=z*(-0.0-w);w=x*A;x=y*A;A=+g[a+176>>2];y=k+ +g[a+184>>2]*(u*r-B*q);q=+g[a+180>>2];r=p+ +g[a+188>>2]*(x*t-w*s);s=+(i+B*A);B=+(j+u*A);g[h>>2]=s;g[h+4>>2]=B;h=c[f>>2]|0;g[h+((c[d>>2]|0)*12|0)+8>>2]=y;d=h+((c[e>>2]|0)*12|0)|0;y=+(n+w*q);w=+(o+x*q);g[d>>2]=y;g[d+4>>2]=w;g[(c[f>>2]|0)+((c[e>>2]|0)*12|0)+8>>2]=r;return}function vi(a,b){a=a|0;b=b|0;var d=0,e=0,f=0,h=0,i=0.0,j=0.0,k=0.0,l=0,m=0,n=0.0,o=0.0,p=0.0,q=0.0,r=0.0,s=0.0,t=0.0,u=0.0,v=0.0,w=0.0,x=0.0,y=0.0,z=0.0,A=0.0,B=0.0,C=0.0,D=0.0,E=0.0,F=0.0,G=0.0,H=0.0,I=0.0,J=0.0;d=a+120|0;e=c[d>>2]|0;f=b+28|0;b=c[f>>2]|0;h=b+(e*12|0)|0;i=+g[h>>2];j=+g[h+4>>2];k=+g[b+(e*12|0)+8>>2];e=a+124|0;l=c[e>>2]|0;m=b+(l*12|0)|0;n=+g[m>>2];o=+g[m+4>>2];p=+g[b+(l*12|0)+8>>2];q=+U(k);r=+T(k);s=+U(p);t=+T(p);u=+g[a+92>>2]- +g[a+160>>2];v=+g[a+96>>2]- +g[a+164>>2];w=r*u-q*v;x=q*u+r*v;v=+g[a+100>>2]- +g[a+168>>2];r=+g[a+104>>2]- +g[a+172>>2];u=t*v-s*r;q=s*v+t*r;r=i+w- +g[a+68>>2];t=j+x- +g[a+72>>2];v=n+u- +g[a+76>>2];s=o+q- +g[a+80>>2];y=+R(r*r+t*t);z=+R(v*v+s*s);if(y>.04999999701976776){A=1.0/y;B=r*A;C=t*A}else{B=0.0;C=0.0}if(z>.04999999701976776){A=1.0/z;D=v*A;E=s*A}else{D=0.0;E=0.0}A=w*C-x*B;s=u*E-q*D;v=+g[a+176>>2];t=+g[a+184>>2];r=+g[a+180>>2];F=+g[a+188>>2];G=+g[a+112>>2];H=v+A*A*t+G*G*(r+s*s*F);if(H>0.0){I=1.0/H}else{I=H}H=+g[a+108>>2]-y-z*G;if(H>0.0){J=H}else{J=-0.0-H}z=H*(-0.0-I);I=-0.0-z;H=B*I;B=C*I;I=z*(-0.0-G);G=D*I;D=E*I;I=+(i+H*v);i=+(j+B*v);g[h>>2]=I;g[h+4>>2]=i;h=c[f>>2]|0;g[h+((c[d>>2]|0)*12|0)+8>>2]=k+(w*B-x*H)*t;d=h+((c[e>>2]|0)*12|0)|0;t=+(n+G*r);n=+(o+D*r);g[d>>2]=t;g[d+4>>2]=n;g[(c[f>>2]|0)+((c[e>>2]|0)*12|0)+8>>2]=p+F*(u*D-q*G);return J<.004999999888241291|0}function wi(a,b){a=a|0;b=b|0;var d=0,e=0.0,f=0.0,h=0.0,i=0.0,j=0.0;d=c[b+48>>2]|0;e=+g[d+24>>2];f=+g[b+92>>2];h=+g[d+20>>2];i=+g[b+96>>2];j=f*h+e*i+ +g[d+16>>2];g[a>>2]=+g[d+12>>2]+(e*f-h*i);g[a+4>>2]=j;return}function xi(a,b){a=a|0;b=b|0;var d=0,e=0.0,f=0.0,h=0.0,i=0.0,j=0.0;d=c[b+52>>2]|0;e=+g[d+24>>2];f=+g[b+100>>2];h=+g[d+20>>2];i=+g[b+104>>2];j=f*h+e*i+ +g[d+16>>2];g[a>>2]=+g[d+12>>2]+(e*f-h*i);g[a+4>>2]=j;return}function yi(a,b,c){a=a|0;b=b|0;c=+c;var d=0.0,e=0.0;d=+g[b+116>>2];e=d*+g[b+140>>2]*c;g[a>>2]=d*+g[b+136>>2]*c;g[a+4>>2]=e;return}function zi(a,b){a=a|0;b=+b;return+0.0}function Ai(a){a=a|0;var b=0,e=0,f=0,j=0,k=0.0;b=i;e=c[(c[a+48>>2]|0)+8>>2]|0;f=c[(c[a+52>>2]|0)+8>>2]|0;gn(2672,(j=i,i=i+1|0,i=i+7&-8,c[j>>2]=0,j)|0);i=j;gn(3384,(j=i,i=i+8|0,c[j>>2]=e,j)|0);i=j;gn(2720,(j=i,i=i+8|0,c[j>>2]=f,j)|0);i=j;gn(2272,(j=i,i=i+8|0,c[j>>2]=d[a+61|0]|0,j)|0);i=j;k=+g[a+72>>2];gn(1792,(j=i,i=i+16|0,h[j>>3]=+g[a+68>>2],h[j+8>>3]=k,j)|0);i=j;k=+g[a+80>>2];gn(1328,(j=i,i=i+16|0,h[j>>3]=+g[a+76>>2],h[j+8>>3]=k,j)|0);i=j;k=+g[a+96>>2];gn(992,(j=i,i=i+16|0,h[j>>3]=+g[a+92>>2],h[j+8>>3]=k,j)|0);i=j;k=+g[a+104>>2];gn(624,(j=i,i=i+16|0,h[j>>3]=+g[a+100>>2],h[j+8>>3]=k,j)|0);i=j;gn(344,(j=i,i=i+8|0,h[j>>3]=+g[a+84>>2],j)|0);i=j;gn(64,(j=i,i=i+8|0,h[j>>3]=+g[a+88>>2],j)|0);i=j;gn(3984,(j=i,i=i+8|0,h[j>>3]=+g[a+112>>2],j)|0);i=j;gn(3784,(j=i,i=i+8|0,c[j>>2]=c[a+56>>2],j)|0);i=j;i=b;return}function Bi(a,b){a=a|0;b=b|0;var c=0,d=0;c=b|0;d=a+68|0;g[d>>2]=+g[d>>2]- +g[c>>2];d=b+4|0;b=a+72|0;g[b>>2]=+g[b>>2]- +g[d>>2];b=a+76|0;g[b>>2]=+g[b>>2]- +g[c>>2];c=a+80|0;g[c>>2]=+g[c>>2]- +g[d>>2];return}function Ci(a){a=a|0;return}function Di(a){a=a|0;Jn(a);return}function Ei(a,b,d,e,f){a=a|0;b=b|0;d=d|0;e=e|0;f=f|0;var g=0,h=0,i=0;g=Vm(f,148)|0;if((g|0)==0){h=0;i=h|0;return i|0}Li(g,a,b,d,e);c[g>>2]=4680;h=g;i=h|0;return i|0}function Fi(a,b){a=a|0;b=b|0;jb[c[(c[a>>2]|0)+4>>2]&255](a);Wm(b,a,148);return}function Gi(a,b,d,e){a=a|0;b=b|0;d=d|0;e=e|0;var f=0,h=0,j=0;f=i;i=i+48|0;h=f|0;j=c[(c[a+48>>2]|0)+12>>2]|0;c[h>>2]=5976;c[h+4>>2]=1;g[h+8>>2]=.009999999776482582;Nn(h+28|0,0,18)|0;Ve(j,h,c[a+56>>2]|0);vf(b,h,d,c[(c[a+52>>2]|0)+12>>2]|0,e);i=f;return}function Hi(a){a=a|0;return}function Ii(a){a=a|0;Jn(a);return}function Ji(b,d,e,f,g){b=b|0;d=d|0;e=e|0;f=f|0;g=g|0;var h=0,i=0,j=0,k=0;if((a[9088]|0)==0){c[2274]=10;c[2275]=34;a[9104]=1;c[2298]=14;c[2299]=30;a[9200]=1;c[2280]=14;c[2281]=30;a[9128]=0;c[2304]=2;c[2305]=52;a[9224]=1;c[2286]=8;c[2287]=96;a[9152]=1;c[2277]=8;c[2278]=96;a[9116]=0;c[2292]=24;c[2293]=122;a[9176]=1;c[2301]=24;c[2302]=122;a[9212]=0;c[2310]=18;c[2311]=104;a[9248]=1;c[2283]=18;c[2284]=104;a[9140]=0;c[2316]=4;c[2317]=72;a[9272]=1;c[2307]=4;c[2308]=72;a[9236]=0;a[9088]=1}h=c[(c[b+12>>2]|0)+4>>2]|0;i=c[(c[e+12>>2]|0)+4>>2]|0;j=c[9096+(h*48|0)+(i*12|0)>>2]|0;if((j|0)==0){k=0;return k|0}if((a[9096+(h*48|0)+(i*12|0)+8|0]|0)==0){k=ub[j&31](e,f,b,d,g)|0;return k|0}else{k=ub[j&31](b,d,e,f,g)|0;return k|0}return 0}function Ki(d,e){d=d|0;e=e|0;var f=0,h=0,i=0,j=0,k=0;f=c[d+48>>2]|0;h=c[d+52>>2]|0;do{if((c[d+124>>2]|0)>0){if((a[f+38|0]|0)!=0){break}if((a[h+38|0]|0)!=0){break}i=c[f+8>>2]|0;j=i+4|0;k=b[j>>1]|0;if((k&2)==0){b[j>>1]=k|2;g[i+160>>2]=0.0}i=c[h+8>>2]|0;k=i+4|0;j=b[k>>1]|0;if(!((j&2)==0)){break}b[k>>1]=j|2;g[i+160>>2]=0.0}}while(0);kb[c[9096+((c[(c[f+12>>2]|0)+4>>2]|0)*48|0)+((c[(c[h+12>>2]|0)+4>>2]|0)*12|0)+4>>2]&255](d,e);return}function Li(a,b,d,e,f){a=a|0;b=b|0;d=d|0;e=e|0;f=f|0;var h=0.0,i=0.0;c[a>>2]=4552;c[a+4>>2]=4;c[a+48>>2]=b;c[a+52>>2]=e;c[a+56>>2]=d;c[a+60>>2]=f;c[a+124>>2]=0;c[a+128>>2]=0;Nn(a+8|0,0,40)|0;g[a+136>>2]=+R(+g[b+16>>2]*+g[e+16>>2]);h=+g[b+20>>2];i=+g[e+20>>2];g[a+140>>2]=h>i?h:i;g[a+144>>2]=0.0;return}function Mi(d,e){d=d|0;e=e|0;var f=0,h=0,j=0,k=0,l=0,m=0,n=0,o=0,p=0,q=0,r=0,s=0,t=0,u=0,v=0,w=0,x=0,y=0,z=0,A=0,B=0,C=0,D=0;f=i;i=i+64|0;h=f|0;j=d+64|0;On(h|0,j|0,64)|0;k=d+4|0;l=c[k>>2]|0;c[k>>2]=l|4;m=l>>>1;l=c[d+48>>2]|0;n=c[d+52>>2]|0;o=(a[n+38|0]|a[l+38|0])<<24>>24!=0;p=c[l+8>>2]|0;q=c[n+8>>2]|0;r=p+12|0;s=q+12|0;do{if(o){t=Af(c[l+12>>2]|0,c[d+56>>2]|0,c[n+12>>2]|0,c[d+60>>2]|0,r,s)|0;c[d+124>>2]=0;u=t;v=m&1}else{wb[c[c[d>>2]>>2]&63](d,j,r,s);t=d+124|0;w=(c[t>>2]|0)>0;if(w){x=c[h+60>>2]|0;y=0;do{z=d+64+(y*20|0)+8|0;g[z>>2]=0.0;A=d+64+(y*20|0)+12|0;g[A>>2]=0.0;B=c[d+64+(y*20|0)+16>>2]|0;C=0;while(1){if((C|0)>=(x|0)){break}if((c[h+(C*20|0)+16>>2]|0)==(B|0)){D=8;break}else{C=C+1|0}}if((D|0)==8){D=0;g[z>>2]=+g[h+(C*20|0)+8>>2];g[A>>2]=+g[h+(C*20|0)+12>>2]}y=y+1|0;}while((y|0)<(c[t>>2]|0))}t=m&1;if(!(w^(t|0)!=0)){u=w;v=t;break}y=p+4|0;x=b[y>>1]|0;if((x&2)==0){b[y>>1]=x|2;g[p+160>>2]=0.0}x=q+4|0;y=b[x>>1]|0;if(!((y&2)==0)){u=w;v=t;break}b[x>>1]=y|2;g[q+160>>2]=0.0;u=w;v=t}}while(0);q=c[k>>2]|0;c[k>>2]=u?q|2:q&-3;q=(v|0)==0;v=u^1;k=(e|0)==0;if(!(q^1|v|k)){kb[c[(c[e>>2]|0)+8>>2]&255](e,d)}if(!(q|u|k)){kb[c[(c[e>>2]|0)+12>>2]&255](e,d)}if(o|v|k){i=f;return}ob[c[(c[e>>2]|0)+32>>2]&31](e,d,h);i=f;return}function Ni(a){a=a|0;return}function Oi(a){a=a|0;Jn(a);return}function Pi(a,b,d,e,f){a=a|0;b=b|0;d=d|0;e=e|0;f=f|0;var g=0,h=0;e=Vm(f,148)|0;if((e|0)==0){g=0;h=g|0;return h|0}Li(e,a,0,d,0);c[e>>2]=4648;g=e;h=g|0;return h|0}function Qi(a,b){a=a|0;b=b|0;jb[c[(c[a>>2]|0)+4>>2]&255](a);Wm(b,a,148);return}function Ri(a,b,d,e){a=a|0;b=b|0;d=d|0;e=e|0;hf(b,c[(c[a+48>>2]|0)+12>>2]|0,d,c[(c[a+52>>2]|0)+12>>2]|0,e);return}function Si(a){a=a|0;return}function Ti(a){a=a|0;Jn(a);return}function Ui(a,b,d,e,f){a=a|0;b=b|0;d=d|0;e=e|0;f=f|0;var g=0,h=0;e=Vm(f,148)|0;if((e|0)==0){g=0;h=g|0;return h|0}Li(e,a,0,d,0);c[e>>2]=5288;g=e;h=g|0;return h|0}function Vi(a,b){a=a|0;b=b|0;jb[c[(c[a>>2]|0)+4>>2]&255](a);Wm(b,a,148);return}function Wi(a,b,d,e){a=a|0;b=b|0;d=d|0;e=e|0;gf(b,c[(c[a+48>>2]|0)+12>>2]|0,d,c[(c[a+52>>2]|0)+12>>2]|0,e);return}function Xi(a){a=a|0;return}function Yi(a){a=a|0;Jn(a);return}function Zi(b,d){b=b|0;d=d|0;var e=0,f=0,h=0,i=0,j=0,k=0,l=0,m=0,n=0,o=0,p=0.0,q=0.0,r=0,s=0,t=0,u=0,v=0,w=0,x=0,y=0;e=b;f=d;c[e>>2]=c[f>>2];c[e+4>>2]=c[f+4>>2];c[e+8>>2]=c[f+8>>2];c[e+12>>2]=c[f+12>>2];c[e+16>>2]=c[f+16>>2];c[e+20>>2]=c[f+20>>2];c[e+24>>2]=c[f+24>>2];f=c[d+44>>2]|0;e=b+36|0;c[e>>2]=f;h=c[d+32>>2]|0;i=b+52|0;c[i>>2]=h;j=b+40|0;c[j>>2]=on(f,h*88|0)|0;h=on(c[e>>2]|0,(c[i>>2]|0)*156|0)|0;e=b+44|0;c[e>>2]=h;c[b+28>>2]=c[d+36>>2];c[b+32>>2]=c[d+40>>2];f=c[d+28>>2]|0;d=b+48|0;c[d>>2]=f;if((c[i>>2]|0)<=0){return}k=b+24|0;l=b+8|0;b=0;m=f;f=h;while(1){h=c[m+(b<<2)>>2]|0;n=c[h+48>>2]|0;o=c[h+52>>2]|0;p=+g[(c[n+12>>2]|0)+8>>2];q=+g[(c[o+12>>2]|0)+8>>2];r=c[n+8>>2]|0;n=c[o+8>>2]|0;o=c[h+124>>2]|0;g[f+(b*156|0)+136>>2]=+g[h+136>>2];g[f+(b*156|0)+140>>2]=+g[h+140>>2];g[f+(b*156|0)+144>>2]=+g[h+144>>2];s=r+8|0;c[f+(b*156|0)+112>>2]=c[s>>2];t=n+8|0;c[f+(b*156|0)+116>>2]=c[t>>2];u=r+136|0;g[f+(b*156|0)+120>>2]=+g[u>>2];v=n+136|0;g[f+(b*156|0)+124>>2]=+g[v>>2];w=r+144|0;g[f+(b*156|0)+128>>2]=+g[w>>2];x=n+144|0;g[f+(b*156|0)+132>>2]=+g[x>>2];c[f+(b*156|0)+152>>2]=b;c[f+(b*156|0)+148>>2]=o;Nn(f+(b*156|0)+80|0,0,32)|0;y=c[j>>2]|0;c[y+(b*88|0)+32>>2]=c[s>>2];c[y+(b*88|0)+36>>2]=c[t>>2];g[y+(b*88|0)+40>>2]=+g[u>>2];g[y+(b*88|0)+44>>2]=+g[v>>2];v=r+44|0;r=y+(b*88|0)+48|0;u=c[v+4>>2]|0;c[r>>2]=c[v>>2];c[r+4>>2]=u;u=n+44|0;n=y+(b*88|0)+56|0;r=c[u+4>>2]|0;c[n>>2]=c[u>>2];c[n+4>>2]=r;g[y+(b*88|0)+64>>2]=+g[w>>2];g[y+(b*88|0)+68>>2]=+g[x>>2];x=h+104|0;w=y+(b*88|0)+16|0;r=c[x+4>>2]|0;c[w>>2]=c[x>>2];c[w+4>>2]=r;r=h+112|0;w=y+(b*88|0)+24|0;x=c[r+4>>2]|0;c[w>>2]=c[r>>2];c[w+4>>2]=x;c[y+(b*88|0)+84>>2]=o;g[y+(b*88|0)+76>>2]=p;g[y+(b*88|0)+80>>2]=q;c[y+(b*88|0)+72>>2]=c[h+120>>2];if((o|0)>0){x=0;do{if((a[k]|0)==0){g[f+(b*156|0)+(x*36|0)+16>>2]=0.0;g[f+(b*156|0)+(x*36|0)+20>>2]=0.0}else{g[f+(b*156|0)+(x*36|0)+16>>2]=+g[l>>2]*+g[h+64+(x*20|0)+8>>2];g[f+(b*156|0)+(x*36|0)+20>>2]=+g[l>>2]*+g[h+64+(x*20|0)+12>>2]}g[f+(b*156|0)+(x*36|0)+24>>2]=0.0;g[f+(b*156|0)+(x*36|0)+28>>2]=0.0;g[f+(b*156|0)+(x*36|0)+32>>2]=0.0;w=h+64+(x*20|0)|0;r=y+(b*88|0)+(x<<3)|0;Nn(f+(b*156|0)+(x*36|0)|0,0,16)|0;n=c[w+4>>2]|0;c[r>>2]=c[w>>2];c[r+4>>2]=n;x=x+1|0;}while((x|0)<(o|0))}o=b+1|0;if((o|0)>=(c[i>>2]|0)){break}b=o;m=c[d>>2]|0;f=c[e>>2]|0}return}function _i(a){a=a|0;var b=0;b=a+36|0;qn(c[b>>2]|0,c[a+44>>2]|0);qn(c[b>>2]|0,c[a+40>>2]|0);return}function $i(a){a=a|0;var b=0,d=0,e=0,f=0,h=0,j=0,k=0,l=0,m=0,n=0,o=0,p=0,q=0,r=0,s=0,t=0,u=0,v=0,w=0,x=0,y=0.0,z=0.0,A=0,B=0,C=0,D=0.0,E=0.0,F=0.0,G=0.0,H=0,I=0.0,J=0.0,K=0.0,L=0.0,M=0.0,N=0.0,O=0.0,P=0,Q=0.0,R=0.0,S=0.0,V=0.0,W=0.0,X=0.0,Y=0.0,Z=0.0,_=0.0,$=0.0,aa=0.0,ba=0.0,ca=0.0,da=0,ea=0,fa=0,ga=0,ha=0.0,ia=0.0,ja=0.0;b=i;i=i+16|0;d=b|0;e=d|0;f=d;h=i;i=i+16|0;j=h|0;k=h;l=i;i=i+32|0;m=l|0;n=l;o=a+52|0;if((c[o>>2]|0)<=0){i=b;return}p=a+44|0;q=a+40|0;r=a+48|0;s=a+28|0;t=a+32|0;a=d+8|0;d=a;u=a+4|0;a=h+8|0;h=a;v=a+4|0;a=l+8|0;l=0;do{w=c[p>>2]|0;x=c[q>>2]|0;y=+g[x+(l*88|0)+76>>2];z=+g[x+(l*88|0)+80>>2];A=(c[(c[r>>2]|0)+(c[w+(l*156|0)+152>>2]<<2)>>2]|0)+64|0;B=c[w+(l*156|0)+112>>2]|0;C=c[w+(l*156|0)+116>>2]|0;D=+g[w+(l*156|0)+120>>2];E=+g[w+(l*156|0)+124>>2];F=+g[w+(l*156|0)+128>>2];G=+g[w+(l*156|0)+132>>2];H=x+(l*88|0)+48|0;I=+g[H>>2];J=+g[H+4>>2];H=x+(l*88|0)+56|0;K=+g[H>>2];L=+g[H+4>>2];H=c[s>>2]|0;x=H+(B*12|0)|0;M=+g[x>>2];N=+g[x+4>>2];O=+g[H+(B*12|0)+8>>2];x=c[t>>2]|0;P=x+(B*12|0)|0;Q=+g[P>>2];R=+g[P+4>>2];S=+g[x+(B*12|0)+8>>2];B=H+(C*12|0)|0;V=+g[B>>2];W=+g[B+4>>2];X=+g[H+(C*12|0)+8>>2];H=x+(C*12|0)|0;Y=+g[H>>2];Z=+g[H+4>>2];_=+g[x+(C*12|0)+8>>2];$=+U(O);g[d>>2]=$;aa=+T(O);g[u>>2]=aa;O=+U(X);g[h>>2]=O;ba=+T(X);g[v>>2]=ba;X=+(M-(I*aa-J*$));ca=+(N-(J*aa+I*$));g[e>>2]=X;g[e+4>>2]=ca;ca=+(V-(K*ba-L*O));X=+(W-(L*ba+K*O));g[j>>2]=ca;g[j+4>>2]=X;yf(n,A,f,y,k,z);A=w+(l*156|0)+72|0;C=A;x=c[m+4>>2]|0;c[C>>2]=c[m>>2];c[C+4>>2]=x;x=w+(l*156|0)+148|0;C=c[x>>2]|0;do{if((C|0)>0){H=w+(l*156|0)+76|0;B=A|0;z=D+E;y=-0.0-_;X=-0.0-S;P=w+(l*156|0)+140|0;da=0;do{ea=a+(da<<3)|0;ca=+g[ea>>2]-M;fa=a+(da<<3)+4|0;ga=w+(l*156|0)+(da*36|0)|0;O=+ca;K=+(+g[fa>>2]-N);g[ga>>2]=O;g[ga+4>>2]=K;K=+g[ea>>2]-V;ea=w+(l*156|0)+(da*36|0)+8|0;O=+K;ba=+(+g[fa>>2]-W);g[ea>>2]=O;g[ea+4>>2]=ba;ba=+g[H>>2];O=+g[w+(l*156|0)+(da*36|0)+4>>2];L=+g[B>>2];$=ca*ba-O*L;I=+g[w+(l*156|0)+(da*36|0)+12>>2];aa=ba*K-L*I;L=z+$*F*$+aa*G*aa;if(L>0.0){ha=1.0/L}else{ha=0.0}g[w+(l*156|0)+(da*36|0)+24>>2]=ha;L=+g[H>>2];aa=+g[B>>2]*-1.0;$=ca*aa-L*O;ba=aa*K-L*I;L=z+$*F*$+ba*G*ba;if(L>0.0){ia=1.0/L}else{ia=0.0}g[w+(l*156|0)+(da*36|0)+28>>2]=ia;ea=w+(l*156|0)+(da*36|0)+32|0;g[ea>>2]=0.0;L=+g[B>>2]*(Y+I*y-Q-O*X)+ +g[H>>2]*(Z+_*K-R-S*ca);if(L<-1.0){g[ea>>2]=L*(-0.0- +g[P>>2])}da=da+1|0;}while((da|0)<(C|0));if((c[x>>2]|0)!=2){break}X=+g[w+(l*156|0)+76>>2];y=+g[A>>2];z=+g[w+(l*156|0)>>2]*X- +g[w+(l*156|0)+4>>2]*y;L=X*+g[w+(l*156|0)+8>>2]-y*+g[w+(l*156|0)+12>>2];ca=X*+g[w+(l*156|0)+36>>2]-y*+g[w+(l*156|0)+40>>2];K=X*+g[w+(l*156|0)+44>>2]-y*+g[w+(l*156|0)+48>>2];y=D+E;X=F*z;O=G*L;I=y+z*X+L*O;L=y+ca*F*ca+K*G*K;z=y+X*ca+O*K;K=I*L-z*z;if(!(I*I<K*1.0e3)){c[x>>2]=1;break}g[w+(l*156|0)+96>>2]=I;g[w+(l*156|0)+100>>2]=z;g[w+(l*156|0)+104>>2]=z;g[w+(l*156|0)+108>>2]=L;if(K!=0.0){ja=1.0/K}else{ja=K}K=z*(-0.0-ja);g[w+(l*156|0)+80>>2]=L*ja;g[w+(l*156|0)+84>>2]=K;g[w+(l*156|0)+88>>2]=K;g[w+(l*156|0)+92>>2]=I*ja}}while(0);l=l+1|0;}while((l|0)<(c[o>>2]|0));i=b;return}function aj(a){a=a|0;var b=0,d=0,e=0,f=0,h=0,i=0,j=0,k=0.0,l=0.0,m=0.0,n=0.0,o=0,p=0,q=0.0,r=0.0,s=0.0,t=0,u=0.0,v=0.0,w=0.0,x=0.0,y=0.0,z=0.0,A=0.0,B=0.0,C=0.0,D=0.0,E=0.0,F=0.0,G=0.0,H=0.0,I=0.0,J=0.0,K=0.0,L=0.0,M=0.0,N=0,O=0.0,P=0.0,Q=0.0,R=0.0,S=0.0,T=0.0;b=a+52|0;if((c[b>>2]|0)<=0){return}d=a+44|0;e=a+32|0;a=0;f=c[e>>2]|0;do{h=c[d>>2]|0;i=c[h+(a*156|0)+112>>2]|0;j=c[h+(a*156|0)+116>>2]|0;k=+g[h+(a*156|0)+120>>2];l=+g[h+(a*156|0)+128>>2];m=+g[h+(a*156|0)+124>>2];n=+g[h+(a*156|0)+132>>2];o=c[h+(a*156|0)+148>>2]|0;p=f+(i*12|0)|0;q=+g[p>>2];r=+g[p+4>>2];s=+g[f+(i*12|0)+8>>2];t=f+(j*12|0)|0;u=+g[t>>2];v=+g[t+4>>2];w=+g[f+(j*12|0)+8>>2];t=h+(a*156|0)+72|0;x=+g[t>>2];y=+g[t+4>>2];z=x*-1.0;if((o|0)>0){A=r;B=q;C=v;D=u;E=s;F=w;t=0;while(1){G=+g[h+(a*156|0)+(t*36|0)+16>>2];H=+g[h+(a*156|0)+(t*36|0)+20>>2];I=x*G+y*H;J=y*G+z*H;H=E-l*(+g[h+(a*156|0)+(t*36|0)>>2]*J- +g[h+(a*156|0)+(t*36|0)+4>>2]*I);G=B-k*I;K=A-k*J;L=F+n*(J*+g[h+(a*156|0)+(t*36|0)+8>>2]-I*+g[h+(a*156|0)+(t*36|0)+12>>2]);M=D+m*I;I=C+m*J;N=t+1|0;if((N|0)<(o|0)){A=K;B=G;C=I;D=M;E=H;F=L;t=N}else{O=K;P=G;Q=I;R=M;S=H;T=L;break}}}else{O=r;P=q;Q=v;R=u;S=s;T=w}F=+P;E=+O;g[p>>2]=F;g[p+4>>2]=E;t=c[e>>2]|0;g[t+(i*12|0)+8>>2]=S;o=t+(j*12|0)|0;E=+R;F=+Q;g[o>>2]=E;g[o+4>>2]=F;f=c[e>>2]|0;g[f+(j*12|0)+8>>2]=T;a=a+1|0;}while((a|0)<(c[b>>2]|0));return}function bj(a){a=a|0;var b=0,d=0,e=0,f=0,h=0,i=0,j=0,k=0,l=0.0,m=0.0,n=0.0,o=0.0,p=0,q=0,r=0,s=0.0,t=0.0,u=0.0,v=0,w=0.0,x=0.0,y=0.0,z=0.0,A=0.0,B=0.0,C=0.0,D=0.0,E=0.0,F=0.0,G=0.0,H=0,I=0.0,J=0.0,K=0.0,L=0.0,M=0.0,N=0.0,O=0.0,P=0,Q=0.0,R=0.0,S=0.0,T=0.0,U=0.0,V=0.0,W=0.0,X=0.0,Y=0.0,Z=0.0,_=0,$=0.0,aa=0.0,ba=0.0,ca=0.0,da=0.0,ea=0.0,fa=0.0,ga=0.0,ha=0.0,ia=0.0,ja=0.0;b=a+52|0;if((c[b>>2]|0)<=0){return}d=a+44|0;e=a+32|0;a=0;f=c[e>>2]|0;do{h=c[d>>2]|0;i=h+(a*156|0)|0;j=c[h+(a*156|0)+112>>2]|0;k=c[h+(a*156|0)+116>>2]|0;l=+g[h+(a*156|0)+120>>2];m=+g[h+(a*156|0)+128>>2];n=+g[h+(a*156|0)+124>>2];o=+g[h+(a*156|0)+132>>2];p=h+(a*156|0)+148|0;q=c[p>>2]|0;r=f+(j*12|0)|0;s=+g[r>>2];t=+g[r+4>>2];u=+g[f+(j*12|0)+8>>2];v=f+(k*12|0)|0;w=+g[v>>2];x=+g[v+4>>2];y=+g[f+(k*12|0)+8>>2];v=h+(a*156|0)+72|0;z=+g[v>>2];A=+g[v+4>>2];B=z*-1.0;C=+g[h+(a*156|0)+136>>2];do{if((q|0)>0){v=h+(a*156|0)+144|0;D=t;E=s;F=x;G=w;H=0;I=y;J=u;do{K=+g[h+(a*156|0)+(H*36|0)+12>>2];L=+g[h+(a*156|0)+(H*36|0)+8>>2];M=+g[h+(a*156|0)+(H*36|0)+4>>2];N=+g[h+(a*156|0)+(H*36|0)>>2];O=C*+g[h+(a*156|0)+(H*36|0)+16>>2];P=h+(a*156|0)+(H*36|0)+20|0;Q=+g[P>>2];R=Q+ +g[h+(a*156|0)+(H*36|0)+28>>2]*(-0.0-(A*(G+K*(-0.0-I)-E-M*(-0.0-J))+B*(F+I*L-D-J*N)- +g[v>>2]));S=-0.0-O;T=R<O?R:O;O=T<S?S:T;T=O-Q;g[P>>2]=O;O=A*T;Q=B*T;E=E-l*O;D=D-l*Q;J=J-m*(N*Q-M*O);G=G+n*O;F=F+n*Q;I=I+o*(L*Q-K*O);H=H+1|0;}while((H|0)<(q|0));if((c[p>>2]|0)!=1){U=J;V=I;W=G;X=F;Y=E;Z=D;_=8;break}O=+g[h+(a*156|0)+12>>2];K=+g[h+(a*156|0)+8>>2];Q=+g[h+(a*156|0)+4>>2];L=+g[i>>2];H=h+(a*156|0)+16|0;M=+g[H>>2];N=M+(z*(G+O*(-0.0-I)-E-Q*(-0.0-J))+A*(F+I*K-D-J*L)- +g[h+(a*156|0)+32>>2])*(-0.0- +g[h+(a*156|0)+24>>2]);T=N>0.0?N:0.0;N=T-M;g[H>>2]=T;T=z*N;M=A*N;$=J-m*(L*M-Q*T);aa=I+o*(K*M-O*T);ba=G+n*T;ca=F+n*M;da=E-l*T;ea=D-l*M}else{U=u;V=y;W=w;X=x;Y=s;Z=t;_=8}}while(0);a:do{if((_|0)==8){_=0;p=h+(a*156|0)+16|0;t=+g[p>>2];q=h+(a*156|0)+52|0;s=+g[q>>2];x=-0.0-V;w=+g[h+(a*156|0)+12>>2];y=+g[h+(a*156|0)+8>>2];u=-0.0-U;B=+g[h+(a*156|0)+4>>2];C=+g[i>>2];M=+g[h+(a*156|0)+48>>2];T=+g[h+(a*156|0)+44>>2];O=+g[h+(a*156|0)+40>>2];K=+g[h+(a*156|0)+36>>2];Q=+g[h+(a*156|0)+104>>2];L=+g[h+(a*156|0)+100>>2];N=z*(W+w*x-Y-B*u)+A*(X+V*y-Z-U*C)- +g[h+(a*156|0)+32>>2]-(t*+g[h+(a*156|0)+96>>2]+s*Q);S=z*(W+M*x-Y-O*u)+A*(X+V*T-Z-U*K)- +g[h+(a*156|0)+68>>2]-(t*L+s*+g[h+(a*156|0)+108>>2]);u=+g[h+(a*156|0)+80>>2]*N+ +g[h+(a*156|0)+88>>2]*S;x=N*+g[h+(a*156|0)+84>>2]+S*+g[h+(a*156|0)+92>>2];R=-0.0-u;fa=-0.0-x;if(!(u>-0.0|x>-0.0)){x=R-t;u=fa-s;ga=z*x;ha=A*x;x=z*u;ia=A*u;u=ga+x;ja=ha+ia;g[p>>2]=R;g[q>>2]=fa;$=U-m*(C*ha-B*ga+(K*ia-O*x));aa=V+o*(y*ha-w*ga+(T*ia-M*x));ba=W+n*u;ca=X+n*ja;da=Y-l*u;ea=Z-l*ja;break}ja=N*(-0.0- +g[h+(a*156|0)+24>>2]);do{if(!(ja<0.0)){if(S+ja*L<0.0){break}u=ja-t;x=0.0-s;ia=z*u;ga=A*u;u=z*x;ha=A*x;x=u+ia;fa=ha+ga;g[p>>2]=ja;g[q>>2]=0.0;$=U-m*(ga*C-ia*B+(ha*K-u*O));aa=V+o*(ga*y-ia*w+(ha*T-u*M));ba=W+n*x;ca=X+n*fa;da=Y-l*x;ea=Z-l*fa;break a}}while(0);ja=S*(-0.0- +g[h+(a*156|0)+60>>2]);do{if(!(ja<0.0)){if(N+ja*Q<0.0){break}L=0.0-t;D=ja-s;E=z*L;F=A*L;L=z*D;G=A*D;D=E+L;I=F+G;g[p>>2]=0.0;g[q>>2]=ja;$=U-m*(F*C-E*B+(G*K-L*O));aa=V+o*(F*y-E*w+(G*T-L*M));ba=W+n*D;ca=X+n*I;da=Y-l*D;ea=Z-l*I;break a}}while(0);if(N<0.0|S<0.0){$=U;aa=V;ba=W;ca=X;da=Y;ea=Z;break}ja=0.0-t;Q=0.0-s;I=z*ja;D=A*ja;ja=z*Q;L=A*Q;Q=I+ja;G=D+L;g[p>>2]=0.0;g[q>>2]=0.0;$=U-m*(D*C-I*B+(L*K-ja*O));aa=V+o*(D*y-I*w+(L*T-ja*M));ba=W+n*Q;ca=X+n*G;da=Y-l*Q;ea=Z-l*G}}while(0);l=+da;n=+ea;g[r>>2]=l;g[r+4>>2]=n;h=c[e>>2]|0;g[h+(j*12|0)+8>>2]=$;i=h+(k*12|0)|0;n=+ba;l=+ca;g[i>>2]=n;g[i+4>>2]=l;f=c[e>>2]|0;g[f+(k*12|0)+8>>2]=aa;a=a+1|0;}while((a|0)<(c[b>>2]|0));return}function cj(a){a=a|0;var b=0,d=0,e=0,f=0,h=0,i=0;b=c[a+52>>2]|0;if((b|0)<=0){return}d=c[a+44>>2]|0;e=c[a+48>>2]|0;a=0;do{f=c[e+(c[d+(a*156|0)+152>>2]<<2)>>2]|0;h=c[d+(a*156|0)+148>>2]|0;if((h|0)>0){i=0;do{g[f+64+(i*20|0)+8>>2]=+g[d+(a*156|0)+(i*36|0)+16>>2];g[f+64+(i*20|0)+12>>2]=+g[d+(a*156|0)+(i*36|0)+20>>2];i=i+1|0;}while((i|0)<(h|0))}a=a+1|0;}while((a|0)<(b|0));return}function dj(a){a=a|0;var b=0,d=0,e=0,f=0,h=0,j=0,k=0,l=0,m=0,n=0.0,o=0,p=0,q=0,r=0,s=0,t=0,u=0,v=0,w=0.0,x=0,y=0,z=0,A=0,B=0,C=0,D=0.0,E=0.0,F=0.0,G=0.0,H=0.0,I=0.0,J=0.0,K=0.0,L=0.0,M=0.0,N=0.0,O=0.0,P=0.0,Q=0.0,R=0.0,S=0.0,V=0.0,W=0.0,X=0.0,Y=0.0,Z=0.0,_=0.0,$=0.0,aa=0.0,ba=0.0,ca=0.0,da=0.0,ea=0.0,fa=0.0,ga=0.0,ha=0.0,ia=0.0,ja=0.0,ka=0.0,la=0.0,ma=0.0,na=0.0,oa=0.0,pa=0.0,qa=0.0,ra=0;b=i;i=i+16|0;d=b|0;e=d|0;f=d;h=i;i=i+16|0;j=h|0;k=h;l=i;i=i+20|0;i=i+7&-8;m=a+52|0;if((c[m>>2]|0)<=0){n=0.0;o=n>=-.014999999664723873;i=b;return o|0}p=a+40|0;q=a+28|0;a=d+8|0;d=a;r=a+4|0;a=h+8|0;h=a;s=a+4|0;a=l;t=l+8|0;u=l+16|0;v=0;w=0.0;x=c[q>>2]|0;while(1){y=c[p>>2]|0;z=y+(v*88|0)|0;A=c[y+(v*88|0)+32>>2]|0;B=c[y+(v*88|0)+36>>2]|0;C=y+(v*88|0)+48|0;D=+g[C>>2];E=+g[C+4>>2];F=+g[y+(v*88|0)+40>>2];G=+g[y+(v*88|0)+64>>2];C=y+(v*88|0)+56|0;H=+g[C>>2];I=+g[C+4>>2];J=+g[y+(v*88|0)+44>>2];K=+g[y+(v*88|0)+68>>2];C=c[y+(v*88|0)+84>>2]|0;y=x+(A*12|0)|0;L=+g[y>>2];M=+g[y+4>>2];N=+g[x+(A*12|0)+8>>2];y=x+(B*12|0)|0;O=+g[y>>2];P=+g[y+4>>2];Q=+g[x+(B*12|0)+8>>2];if((C|0)>0){R=F+J;S=M;V=L;W=P;X=O;y=0;Y=Q;Z=N;_=w;do{$=+U(Z);g[d>>2]=$;aa=+T(Z);g[r>>2]=aa;ba=+U(Y);g[h>>2]=ba;ca=+T(Y);g[s>>2]=ca;da=+(V-(D*aa-E*$));ea=+(S-(E*aa+D*$));g[e>>2]=da;g[e+4>>2]=ea;ea=+(X-(H*ca-I*ba));da=+(W-(I*ca+H*ba));g[j>>2]=ea;g[j+4>>2]=da;ej(l,z,f,k,y);da=+g[a>>2];ea=+g[a+4>>2];ba=+g[t>>2];ca=+g[t+4>>2];$=+g[u>>2];aa=ba-V;fa=ca-S;ga=ba-X;ba=ca-W;_=_<$?_:$;ca=($+.004999999888241291)*.20000000298023224;$=ca<0.0?ca:0.0;ca=ea*aa-da*fa;ha=ea*ga-da*ba;ia=ha*K*ha+(R+ca*G*ca);if(ia>0.0){ja=(-0.0-($<-.20000000298023224?-.20000000298023224:$))/ia}else{ja=0.0}ia=da*ja;da=ea*ja;V=V-F*ia;S=S-F*da;Z=Z-G*(aa*da-fa*ia);X=X+J*ia;W=W+J*da;Y=Y+K*(ga*da-ba*ia);y=y+1|0;}while((y|0)<(C|0));ka=S;la=V;ma=W;na=X;oa=Y;pa=Z;qa=_;ra=c[q>>2]|0}else{ka=M;la=L;ma=P;na=O;oa=Q;pa=N;qa=w;ra=x}C=ra+(A*12|0)|0;K=+la;J=+ka;g[C>>2]=K;g[C+4>>2]=J;C=c[q>>2]|0;g[C+(A*12|0)+8>>2]=pa;y=C+(B*12|0)|0;J=+na;K=+ma;g[y>>2]=J;g[y+4>>2]=K;y=c[q>>2]|0;g[y+(B*12|0)+8>>2]=oa;C=v+1|0;if((C|0)<(c[m>>2]|0)){v=C;w=qa;x=y}else{n=qa;break}}o=n>=-.014999999664723873;i=b;return o|0}function ej(a,b,d,e,f){a=a|0;b=b|0;d=d|0;e=e|0;f=f|0;var h=0,i=0.0,j=0.0,k=0.0,l=0.0,m=0.0,n=0.0,o=0.0,p=0.0,q=0,r=0.0,s=0.0,t=0.0,u=0,v=0,w=0.0;h=c[b+72>>2]|0;if((h|0)==0){i=+g[d+12>>2];j=+g[b+24>>2];k=+g[d+8>>2];l=+g[b+28>>2];m=+g[d>>2]+(i*j-k*l);n=j*k+i*l+ +g[d+4>>2];l=+g[e+12>>2];i=+g[b>>2];k=+g[e+8>>2];j=+g[b+4>>2];o=+g[e>>2]+(l*i-k*j);p=i*k+l*j+ +g[e+4>>2];j=o-m;l=p-n;q=a;k=+j;i=+l;g[q>>2]=k;g[q+4>>2]=i;i=+R(j*j+l*l);if(i<1.1920928955078125e-7){r=j;s=l}else{k=1.0/i;i=j*k;g[a>>2]=i;t=l*k;g[a+4>>2]=t;r=i;s=t}q=a+8|0;t=+((m+o)*.5);o=+((n+p)*.5);g[q>>2]=t;g[q+4>>2]=o;g[a+16>>2]=j*r+l*s- +g[b+76>>2]- +g[b+80>>2];return}else if((h|0)==2){q=e+12|0;s=+g[q>>2];l=+g[b+16>>2];u=e+8|0;r=+g[u>>2];j=+g[b+20>>2];o=s*l-r*j;t=l*r+s*j;v=a;j=+o;s=+t;g[v>>2]=j;g[v+4>>2]=s;s=+g[q>>2];j=+g[b+24>>2];r=+g[u>>2];l=+g[b+28>>2];p=+g[d+12>>2];n=+g[b+(f<<3)>>2];m=+g[d+8>>2];i=+g[b+(f<<3)+4>>2];k=+g[d>>2]+(p*n-m*i);w=n*m+p*i+ +g[d+4>>2];g[a+16>>2]=o*(k-(+g[e>>2]+(s*j-r*l)))+(w-(j*r+s*l+ +g[e+4>>2]))*t- +g[b+76>>2]- +g[b+80>>2];u=a+8|0;l=+k;k=+w;g[u>>2]=l;g[u+4>>2]=k;k=+(-0.0-o);o=+(-0.0-t);g[v>>2]=k;g[v+4>>2]=o;return}else if((h|0)==1){h=d+12|0;o=+g[h>>2];k=+g[b+16>>2];v=d+8|0;t=+g[v>>2];l=+g[b+20>>2];w=o*k-t*l;s=k*t+o*l;u=a;l=+w;o=+s;g[u>>2]=l;g[u+4>>2]=o;o=+g[h>>2];l=+g[b+24>>2];t=+g[v>>2];k=+g[b+28>>2];r=+g[e+12>>2];j=+g[b+(f<<3)>>2];i=+g[e+8>>2];p=+g[b+(f<<3)+4>>2];m=+g[e>>2]+(r*j-i*p);n=j*i+r*p+ +g[e+4>>2];g[a+16>>2]=w*(m-(+g[d>>2]+(o*l-t*k)))+(n-(l*t+o*k+ +g[d+4>>2]))*s- +g[b+76>>2]- +g[b+80>>2];b=a+8|0;s=+m;m=+n;g[b>>2]=s;g[b+4>>2]=m;return}else{g[a+16>>2]=0.0;b=a;d=8600;e=c[d+4>>2]|0;c[b>>2]=c[d>>2];c[b+4>>2]=e;e=a+8|0;a=8600;b=c[a+4>>2]|0;c[e>>2]=c[a>>2];c[e+4>>2]=b;return}}function fj(a,b,d){a=a|0;b=b|0;d=d|0;var e=0,f=0,h=0,j=0,k=0,l=0,m=0,n=0,o=0,p=0.0,q=0,r=0,s=0,t=0,u=0,v=0,w=0,x=0,y=0.0,z=0,A=0,B=0,C=0,D=0,E=0.0,F=0.0,G=0.0,H=0.0,I=0.0,J=0.0,K=0.0,L=0.0,M=0,N=0.0,O=0.0,P=0.0,Q=0.0,R=0.0,S=0.0,V=0.0,W=0.0,X=0.0,Y=0.0,Z=0.0,_=0.0,$=0.0,aa=0.0,ba=0.0,ca=0.0,da=0.0,ea=0.0,fa=0.0,ga=0.0,ha=0.0,ia=0.0,ja=0.0,ka=0.0,la=0.0,ma=0.0,na=0.0,oa=0.0,pa=0.0,qa=0.0,ra=0.0,sa=0.0,ta=0;e=i;i=i+16|0;f=e|0;h=f|0;j=f;k=i;i=i+16|0;l=k|0;m=k;n=i;i=i+20|0;i=i+7&-8;o=a+52|0;if((c[o>>2]|0)<=0){p=0.0;q=p>=-.007499999832361937;i=e;return q|0}r=a+40|0;s=a+28|0;a=f+8|0;f=a;t=a+4|0;a=k+8|0;k=a;u=a+4|0;a=n;v=n+8|0;w=n+16|0;x=0;y=0.0;while(1){z=c[r>>2]|0;A=z+(x*88|0)|0;B=c[z+(x*88|0)+32>>2]|0;C=c[z+(x*88|0)+36>>2]|0;D=z+(x*88|0)+48|0;E=+g[D>>2];F=+g[D+4>>2];D=z+(x*88|0)+56|0;G=+g[D>>2];H=+g[D+4>>2];D=c[z+(x*88|0)+84>>2]|0;if((B|0)==(b|0)|(B|0)==(d|0)){I=+g[z+(x*88|0)+40>>2];J=+g[z+(x*88|0)+64>>2]}else{I=0.0;J=0.0}if((C|0)==(b|0)|(C|0)==(d|0)){K=+g[z+(x*88|0)+44>>2];L=+g[z+(x*88|0)+68>>2]}else{K=0.0;L=0.0}z=c[s>>2]|0;M=z+(B*12|0)|0;N=+g[M>>2];O=+g[M+4>>2];P=+g[z+(B*12|0)+8>>2];M=z+(C*12|0)|0;Q=+g[M>>2];R=+g[M+4>>2];S=+g[z+(C*12|0)+8>>2];if((D|0)>0){V=I+K;W=O;X=N;Y=R;Z=Q;_=P;$=S;M=0;aa=y;do{ba=+U(_);g[f>>2]=ba;ca=+T(_);g[t>>2]=ca;da=+U($);g[k>>2]=da;ea=+T($);g[u>>2]=ea;fa=+(X-(E*ca-F*ba));ga=+(W-(F*ca+E*ba));g[h>>2]=fa;g[h+4>>2]=ga;ga=+(Z-(G*ea-H*da));fa=+(Y-(H*ea+G*da));g[l>>2]=ga;g[l+4>>2]=fa;ej(n,A,j,m,M);fa=+g[a>>2];ga=+g[a+4>>2];da=+g[v>>2];ea=+g[v+4>>2];ba=+g[w>>2];ca=da-X;ha=ea-W;ia=da-Z;da=ea-Y;aa=aa<ba?aa:ba;ea=(ba+.004999999888241291)*.75;ba=ea<0.0?ea:0.0;ea=ga*ca-fa*ha;ja=ga*ia-fa*da;ka=ja*L*ja+(V+ea*J*ea);if(ka>0.0){la=(-0.0-(ba<-.20000000298023224?-.20000000298023224:ba))/ka}else{la=0.0}ka=fa*la;fa=ga*la;X=X-I*ka;W=W-I*fa;_=_-J*(ca*fa-ha*ka);Z=Z+K*ka;Y=Y+K*fa;$=$+L*(ia*fa-da*ka);M=M+1|0;}while((M|0)<(D|0));ma=W;na=X;oa=Y;pa=Z;qa=_;ra=$;sa=aa;ta=c[s>>2]|0}else{ma=O;na=N;oa=R;pa=Q;qa=P;ra=S;sa=y;ta=z}D=ta+(B*12|0)|0;V=+na;G=+ma;g[D>>2]=V;g[D+4>>2]=G;D=c[s>>2]|0;g[D+(B*12|0)+8>>2]=qa;M=D+(C*12|0)|0;G=+pa;V=+oa;g[M>>2]=G;g[M+4>>2]=V;g[(c[s>>2]|0)+(C*12|0)+8>>2]=ra;M=x+1|0;if((M|0)<(c[o>>2]|0)){x=M;y=sa}else{p=sa;break}}q=p>=-.007499999832361937;i=e;return q|0}function gj(a,b,d,e,f){a=a|0;b=b|0;d=d|0;e=e|0;f=f|0;var g=0,h=0;e=Vm(f,148)|0;if((e|0)==0){g=0;h=g|0;return h|0}Li(e,a,0,d,0);c[e>>2]=4832;g=e;h=g|0;return h|0}function hj(a,b){a=a|0;b=b|0;jb[c[(c[a>>2]|0)+4>>2]&255](a);Wm(b,a,148);return}function ij(a,b,d,e){a=a|0;b=b|0;d=d|0;e=e|0;sf(b,c[(c[a+48>>2]|0)+12>>2]|0,d,c[(c[a+52>>2]|0)+12>>2]|0,e);return}function jj(a){a=a|0;return}function kj(a){a=a|0;Jn(a);return}function lj(a,b,d,e,f){a=a|0;b=b|0;d=d|0;e=e|0;f=f|0;var g=0,h=0,i=0;g=Vm(f,148)|0;if((g|0)==0){h=0;i=h|0;return i|0}Li(g,a,b,d,e);c[g>>2]=4744;h=g;i=h|0;return i|0}function mj(a,b){a=a|0;b=b|0;jb[c[(c[a>>2]|0)+4>>2]&255](a);Wm(b,a,148);return}function nj(a,b,d,e){a=a|0;b=b|0;d=d|0;e=e|0;var f=0,h=0,j=0;f=i;i=i+48|0;h=f|0;j=c[(c[a+48>>2]|0)+12>>2]|0;c[h>>2]=5976;c[h+4>>2]=1;g[h+8>>2]=.009999999776482582;Nn(h+28|0,0,18)|0;Ve(j,h,c[a+56>>2]|0);sf(b,h,d,c[(c[a+52>>2]|0)+12>>2]|0,e);i=f;return}function oj(a){a=a|0;return}function pj(a){a=a|0;Jn(a);return}function qj(a,b,d,e,f){a=a|0;b=b|0;d=d|0;e=e|0;f=f|0;var g=0,h=0;e=Vm(f,148)|0;if((e|0)==0){g=0;h=g|0;return h|0}Li(e,a,0,d,0);c[e>>2]=4712;g=e;h=g|0;return h|0}function rj(a,b){a=a|0;b=b|0;jb[c[(c[a>>2]|0)+4>>2]&255](a);Wm(b,a,148);return}function sj(a,b,d,e){a=a|0;b=b|0;d=d|0;e=e|0;vf(b,c[(c[a+48>>2]|0)+12>>2]|0,d,c[(c[a+52>>2]|0)+12>>2]|0,e);return}function tj(a){a=a|0;return}function uj(a){a=a|0;Jn(a);return}function vj(a,b,d,e,f){a=a|0;b=b|0;d=d|0;e=e|0;f=f|0;var g=0,h=0;e=Vm(f,148)|0;if((e|0)==0){g=0;h=g|0;return h|0}Li(e,a,0,d,0);c[e>>2]=5024;g=e;h=g|0;return h|0}function wj(a,b){a=a|0;b=b|0;jb[c[(c[a>>2]|0)+4>>2]&255](a);Wm(b,a,148);return}function xj(a,b,d,e){a=a|0;b=b|0;d=d|0;e=e|0;wf(b,c[(c[a+48>>2]|0)+12>>2]|0,d,c[(c[a+52>>2]|0)+12>>2]|0,e);return}function yj(a){a=a|0;return}function zj(a){a=a|0;Jn(a);return}function Aj(a){a=a|0;b[a+32>>1]=1;b[a+34>>1]=-1;b[a+36>>1]=0;c[a+40>>2]=0;c[a+24>>2]=0;c[a+28>>2]=0;Nn(a|0,0,16)|0;return}function Bj(d,e,f,h){d=d|0;e=e|0;f=f|0;h=h|0;var i=0,j=0,k=0,l=0,m=0,n=0.0,o=0;c[d+40>>2]=c[h+4>>2];g[d+16>>2]=+g[h+8>>2];g[d+20>>2]=+g[h+12>>2];c[d+8>>2]=f;c[d+4>>2]=0;f=d+32|0;i=h+22|0;b[f>>1]=b[i>>1]|0;b[f+2>>1]=b[i+2>>1]|0;b[f+4>>1]=b[i+4>>1]|0;a[d+38|0]=a[h+20|0]|0;i=c[h>>2]|0;f=tb[c[(c[i>>2]|0)+8>>2]&63](i,e)|0;c[d+12>>2]=f;i=mb[c[(c[f>>2]|0)+12>>2]&15](f)|0;f=Vm(e,i*28|0)|0;e=d+24|0;c[e>>2]=f;if((i|0)>0){j=0;k=f}else{l=d+28|0;c[l>>2]=0;m=h+16|0;n=+g[m>>2];o=d|0;g[o>>2]=n;return}do{c[k+(j*28|0)+16>>2]=0;k=c[e>>2]|0;c[k+(j*28|0)+24>>2]=-1;j=j+1|0;}while((j|0)<(i|0));l=d+28|0;c[l>>2]=0;m=h+16|0;n=+g[m>>2];o=d|0;g[o>>2]=n;return}function Cj(a,b){a=a|0;b=b|0;var d=0,e=0,f=0;d=a+12|0;e=c[d>>2]|0;f=mb[c[(c[e>>2]|0)+12>>2]&15](e)|0;e=a+24|0;Wm(b,c[e>>2]|0,f*28|0);c[e>>2]=0;e=c[d>>2]|0;f=c[e+4>>2]|0;if((f|0)==2){jb[c[c[e>>2]>>2]&255](e);Wm(b,e,152);c[d>>2]=0;return}else if((f|0)==1){jb[c[c[e>>2]>>2]&255](e);Wm(b,e,48);c[d>>2]=0;return}else if((f|0)==0){jb[c[c[e>>2]>>2]&255](e);Wm(b,e,20);c[d>>2]=0;return}else if((f|0)==3){jb[c[c[e>>2]>>2]&255](e);Wm(b,e,40);c[d>>2]=0;return}else{c[d>>2]=0;return}}function Dj(a,b,d){a=a|0;b=b|0;d=d|0;var e=0,f=0,g=0,h=0,i=0,j=0,k=0,l=0;e=a+12|0;f=c[e>>2]|0;g=mb[c[(c[f>>2]|0)+12>>2]&15](f)|0;f=a+28|0;c[f>>2]=g;if((g|0)<=0){return}g=a+24|0;h=0;do{i=c[g>>2]|0;j=i+(h*28|0)|0;k=c[e>>2]|0;l=j|0;wb[c[(c[k>>2]|0)+28>>2]&63](k,l,d,h);c[i+(h*28|0)+24>>2]=bf(b,l,j)|0;c[i+(h*28|0)+16>>2]=a;c[i+(h*28|0)+20>>2]=h;h=h+1|0;}while((h|0)<(c[f>>2]|0));return}function Ej(a,b){a=a|0;b=b|0;var d=0,e=0,f=0;d=a+28|0;if((c[d>>2]|0)<=0){c[d>>2]=0;return}e=a+24|0;a=0;do{f=(c[e>>2]|0)+(a*28|0)+24|0;cf(b,c[f>>2]|0);c[f>>2]=-1;a=a+1|0;}while((a|0)<(c[d>>2]|0));c[d>>2]=0;return}function Fj(a,b,d,e){a=a|0;b=b|0;d=d|0;e=e|0;var f=0,h=0,j=0,k=0,l=0,m=0,n=0,o=0,p=0,q=0,r=0,s=0,t=0,u=0,v=0,w=0,x=0,y=0,z=0,A=0,B=0,C=0,D=0,E=0,F=0.0,G=0.0,H=0.0,I=0.0,J=0.0;f=i;i=i+40|0;h=f|0;j=f+16|0;k=f+32|0;l=a+28|0;if((c[l>>2]|0)<=0){i=f;return}m=a+24|0;n=a+12|0;a=h|0;o=j|0;p=h+4|0;q=j+4|0;r=h+8|0;s=j+8|0;t=h+12|0;u=j+12|0;v=e|0;w=d|0;x=e+4|0;y=d+4|0;z=k|0;A=k+4|0;B=0;do{C=c[m>>2]|0;D=c[n>>2]|0;E=C+(B*28|0)+20|0;wb[c[(c[D>>2]|0)+28>>2]&63](D,h,d,c[E>>2]|0);D=c[n>>2]|0;wb[c[(c[D>>2]|0)+28>>2]&63](D,j,e,c[E>>2]|0);E=C+(B*28|0)|0;F=+g[a>>2];G=+g[o>>2];H=+g[p>>2];I=+g[q>>2];D=E;J=+(F<G?F:G);G=+(H<I?H:I);g[D>>2]=J;g[D+4>>2]=G;G=+g[r>>2];J=+g[s>>2];I=+g[t>>2];H=+g[u>>2];D=C+(B*28|0)+8|0;F=+(G>J?G:J);J=+(I>H?I:H);g[D>>2]=F;g[D+4>>2]=J;J=+g[x>>2]- +g[y>>2];g[z>>2]=+g[v>>2]- +g[w>>2];g[A>>2]=J;df(b,c[C+(B*28|0)+24>>2]|0,E,k);B=B+1|0;}while((B|0)<(c[l>>2]|0));i=f;return}function Gj(b,d){b=b|0;d=d|0;var e=0,f=0,h=0;e=b|0;Tm(e);mn(b+76|0);fk(b+102880|0);c[b+102992>>2]=0;c[b+102996>>2]=0;Nn(b+102960|0,0,20)|0;a[b+103004|0]=1;a[b+103005|0]=1;a[b+103006|0]=0;a[b+103007|0]=1;a[b+102988|0]=1;f=d;d=b+102980|0;h=c[f+4>>2]|0;c[d>>2]=c[f>>2];c[d+4>>2]=h;c[b+102876>>2]=4;g[b+103e3>>2]=0.0;c[b+102956>>2]=e;c[b+103040>>2]=32;c[b+103044>>2]=c[6];Nn(b+103008|0,0,32)|0;return}function Hj(a){a=a|0;var b=0,d=0,e=0,f=0,g=0,h=0,i=0,j=0,k=0;b=c[a+102960>>2]|0;if((b|0)!=0){d=a|0;e=b;while(1){b=c[e+112>>2]|0;f=c[e+116>>2]|0;while(1){if((f|0)==0){break}g=c[f+4>>2]|0;c[f+28>>2]=0;Cj(f,d);f=g}if((b|0)==0){break}else{e=b}}}e=a+102968|0;d=c[e>>2]|0;if((d|0)!=0){f=a+102876|0;g=a|0;h=d;while(1){if((c[f>>2]&2|0)==0){d=h+404|0;i=c[d>>2]|0;j=h+408|0;if((i|0)!=0){c[i+408>>2]=c[j>>2]}i=c[j>>2]|0;if((i|0)!=0){c[i+404>>2]=c[d>>2]}if((c[e>>2]|0)==(h|0)){c[e>>2]=c[j>>2]}Hk(h);Wm(g,h|0,416);k=c[e>>2]|0}else{k=h}if((k|0)==0){break}else{h=k}}}af(a+102880|0);nn(a+76|0);Um(a|0);return}function Ij(a,b){a=a|0;b=b|0;var d=0,e=0,f=0;if((c[a+102876>>2]&2|0)!=0){return}d=b+404|0;e=c[d>>2]|0;f=b+408|0;if((e|0)!=0){c[e+408>>2]=c[f>>2]}e=c[f>>2]|0;if((e|0)!=0){c[e+404>>2]=c[d>>2]}d=a+102968|0;if((c[d>>2]|0)==(b|0)){c[d>>2]=c[f>>2]}Hk(b);Wm(a|0,b|0,416);return}function Jj(a,b){a=a|0;b=b|0;c[a+102952>>2]=b;return}function Kj(a,b){a=a|0;b=b|0;var d=0,e=0,f=0,g=0;if((c[a+102876>>2]&2|0)!=0){d=0;return d|0}e=Vm(a|0,168)|0;if((e|0)==0){f=0}else{g=e;Wj(g,b,a);f=g}c[f+108>>2]=0;g=a+102960|0;c[f+112>>2]=c[g>>2];b=c[g>>2]|0;if((b|0)!=0){c[b+108>>2]=f}c[g>>2]=f;g=a+102972|0;c[g>>2]=(c[g>>2]|0)+1;d=f;return d|0}function Lj(a,b){a=a|0;b=b|0;var d=0,e=0,f=0,g=0,h=0,i=0,j=0,k=0,l=0;if((c[a+102876>>2]&2|0)!=0){return}d=b+124|0;e=c[d>>2]|0;if((e|0)!=0){f=a+102992|0;g=e;while(1){e=c[g+12>>2]|0;h=c[f>>2]|0;if((h|0)==0){i=g+4|0}else{j=g+4|0;kb[c[(c[h>>2]|0)+8>>2]&255](h,c[j>>2]|0);i=j}Mj(a,c[i>>2]|0);c[d>>2]=e;if((e|0)==0){break}else{g=e}}}c[d>>2]=0;d=b+128|0;g=c[d>>2]|0;if((g|0)!=0){i=a+102880|0;f=g;while(1){g=c[f+12>>2]|0;gk(i,c[f+4>>2]|0);if((g|0)==0){break}else{f=g}}}c[d>>2]=0;d=b+116|0;f=c[d>>2]|0;if((f|0)==0){k=b+120|0}else{i=a+102992|0;g=a+102880|0;e=a|0;j=b+120|0;h=f;while(1){f=c[h+4>>2]|0;l=c[i>>2]|0;if((l|0)!=0){kb[c[(c[l>>2]|0)+12>>2]&255](l,h)}Ej(h,g);Cj(h,e);Wm(e,h,44);c[d>>2]=f;c[j>>2]=(c[j>>2]|0)-1;if((f|0)==0){k=j;break}else{h=f}}}c[d>>2]=0;c[k>>2]=0;k=b+108|0;d=c[k>>2]|0;h=b+112|0;if((d|0)!=0){c[d+112>>2]=c[h>>2]}d=c[h>>2]|0;if((d|0)!=0){c[d+108>>2]=c[k>>2]}k=a+102960|0;if((c[k>>2]|0)==(b|0)){c[k>>2]=c[h>>2]}h=a+102972|0;c[h>>2]=(c[h>>2]|0)-1;Xj(b);Wm(a|0,b,168);return}function Mj(d,e){d=d|0;e=e|0;var f=0,h=0,i=0,j=0,k=0,l=0,m=0;if((c[d+102876>>2]&2|0)!=0){return}f=a[e+61|0]|0;h=e+8|0;i=c[h>>2]|0;j=e+12|0;if((i|0)!=0){c[i+12>>2]=c[j>>2]}i=c[j>>2]|0;if((i|0)!=0){c[i+8>>2]=c[h>>2]}h=d+102964|0;if((c[h>>2]|0)==(e|0)){c[h>>2]=c[j>>2]}j=c[e+48>>2]|0;h=c[e+52>>2]|0;i=j+4|0;k=b[i>>1]|0;if((k&2)==0){b[i>>1]=k|2;g[j+160>>2]=0.0}k=h+4|0;i=b[k>>1]|0;if((i&2)==0){b[k>>1]=i|2;g[h+160>>2]=0.0}i=e+24|0;k=c[i>>2]|0;l=e+28|0;if((k|0)!=0){c[k+12>>2]=c[l>>2]}k=c[l>>2]|0;if((k|0)!=0){c[k+8>>2]=c[i>>2]}k=j+124|0;if((e+16|0)==(c[k>>2]|0)){c[k>>2]=c[l>>2]}c[i>>2]=0;c[l>>2]=0;l=e+40|0;i=c[l>>2]|0;k=e+44|0;if((i|0)!=0){c[i+12>>2]=c[k>>2]}i=c[k>>2]|0;if((i|0)!=0){c[i+8>>2]=c[l>>2]}i=h+124|0;if((e+32|0)==(c[i>>2]|0)){c[i>>2]=c[k>>2]}c[l>>2]=0;c[k>>2]=0;Jh(e,d|0);e=d+102976|0;c[e>>2]=(c[e>>2]|0)-1;if(!(f<<24>>24==0)){return}f=c[h+128>>2]|0;if((f|0)==0){return}else{m=f}do{if((c[m>>2]|0)==(j|0)){f=(c[m+4>>2]|0)+4|0;c[f>>2]=c[f>>2]|8}m=c[m+12>>2]|0;}while((m|0)!=0);return}function Nj(b,d){b=b|0;d=d|0;var e=0,f=0,g=0,h=0,i=0,j=0,k=0,l=0,m=0,n=0;if((c[b+102876>>2]&2|0)!=0){e=0;return e|0}f=Ih(d,b|0)|0;c[f+8>>2]=0;g=b+102964|0;c[f+12>>2]=c[g>>2];h=c[g>>2]|0;if((h|0)!=0){c[h+8>>2]=f}c[g>>2]=f;g=b+102976|0;c[g>>2]=(c[g>>2]|0)+1;g=f+16|0;c[f+20>>2]=f;b=f+52|0;c[g>>2]=c[b>>2];c[f+24>>2]=0;h=f+48|0;i=c[h>>2]|0;j=i+124|0;c[f+28>>2]=c[j>>2];k=c[j>>2]|0;if((k|0)==0){l=i}else{c[k+8>>2]=g;l=c[h>>2]|0}c[l+124>>2]=g;g=f+32|0;c[f+36>>2]=f;c[g>>2]=c[h>>2];c[f+40>>2]=0;h=c[b>>2]|0;l=h+124|0;c[f+44>>2]=c[l>>2];k=c[l>>2]|0;if((k|0)==0){m=h}else{c[k+8>>2]=g;m=c[b>>2]|0}c[m+124>>2]=g;g=c[d+8>>2]|0;if((a[d+16|0]|0)!=0){e=f;return e|0}m=c[(c[d+12>>2]|0)+128>>2]|0;if((m|0)==0){e=f;return e|0}else{n=m}while(1){if((c[n>>2]|0)==(g|0)){m=(c[n+4>>2]|0)+4|0;c[m>>2]=c[m>>2]|8}m=c[n+12>>2]|0;if((m|0)==0){e=f;break}else{n=m}}return e|0}function Oj(a,b){a=a|0;b=b|0;var d=0,e=0,f=0,g=0;if((c[a+102876>>2]&2|0)!=0){d=0;return d|0}e=Vm(a|0,416)|0;if((e|0)==0){f=0}else{g=e;Gk(g,b,a);f=g}c[f+404>>2]=0;g=a+102968|0;c[f+408>>2]=c[g>>2];a=c[g>>2]|0;if((a|0)!=0){c[a+404>>2]=f}c[g>>2]=f;d=f;return d|0}function Pj(d,e){d=d|0;e=e|0;var f=0,h=0,j=0,k=0,l=0,m=0,n=0,o=0,p=0,q=0,r=0,s=0,t=0,u=0,v=0,w=0,x=0,y=0,z=0,A=0,B=0,C=0,D=0,E=0,F=0,G=0,H=0,I=0,J=0,K=0,L=0,M=0,N=0,O=0,P=0,Q=0,R=0,S=0,T=0,U=0,V=0,W=0,X=0;f=i;i=i+96|0;h=f|0;j=f+56|0;k=f+88|0;l=d+102960|0;m=c[l>>2]|0;if((m|0)!=0){n=m;do{m=n+28|0;o=n+12|0;c[m>>2]=c[o>>2];c[m+4>>2]=c[o+4>>2];c[m+8>>2]=c[o+8>>2];c[m+12>>2]=c[o+12>>2];n=c[n+112>>2]|0;}while((n|0)!=0)}n=d+103020|0;g[n>>2]=0.0;o=d+103024|0;g[o>>2]=0.0;m=d+103028|0;g[m>>2]=0.0;p=d+102972|0;q=d+102880|0;r=d+76|0;Bf(h,c[p>>2]|0,c[d+102944>>2]|0,c[d+102976>>2]|0,r,c[d+102952>>2]|0);s=c[l>>2]|0;if((s|0)!=0){t=s;do{s=t+4|0;b[s>>1]=b[s>>1]&-2;t=c[t+112>>2]|0;}while((t|0)!=0)}t=c[d+102940>>2]|0;if((t|0)!=0){s=t;do{t=s+4|0;c[t>>2]=c[t>>2]&-2;s=c[s+12>>2]|0;}while((s|0)!=0)}s=c[d+102964>>2]|0;if((s|0)!=0){t=s;do{a[t+60|0]=0;t=c[t+12>>2]|0;}while((t|0)!=0)}t=on(r,c[p>>2]<<2)|0;p=t;s=c[l>>2]|0;if((s|0)!=0){u=h+28|0;v=h+36|0;w=h+32|0;x=h+8|0;y=h+16|0;z=h+12|0;A=d+102980|0;B=d+102988|0;C=j+12|0;D=j+16|0;E=j+20|0;F=s;do{s=F+4|0;G=b[s>>1]|0;do{if((G&35)==34){if((c[F>>2]|0)==0){break}c[u>>2]=0;c[v>>2]=0;c[w>>2]=0;c[p>>2]=F;b[s>>1]=G|1;H=1;I=0;while(1){J=H-1|0;K=c[p+(J<<2)>>2]|0;c[K+8>>2]=I;L=c[u>>2]|0;c[(c[x>>2]|0)+(L<<2)>>2]=K;c[u>>2]=L+1;L=K+4|0;M=b[L>>1]|0;if((M&2)==0){b[L>>1]=M|2;g[K+160>>2]=0.0}do{if((c[K>>2]|0)==0){N=J}else{M=c[K+128>>2]|0;if((M|0)==0){O=J}else{L=J;P=M;while(1){M=c[P+4>>2]|0;Q=M+4|0;do{if((c[Q>>2]&7|0)==6){if((a[(c[M+48>>2]|0)+38|0]|0)!=0){R=L;break}if((a[(c[M+52>>2]|0)+38|0]|0)!=0){R=L;break}S=c[v>>2]|0;c[v>>2]=S+1;c[(c[z>>2]|0)+(S<<2)>>2]=M;c[Q>>2]=c[Q>>2]|1;S=c[P>>2]|0;T=S+4|0;U=b[T>>1]|0;if(!((U&1)==0)){R=L;break}c[p+(L<<2)>>2]=S;b[T>>1]=U|1;R=L+1|0}else{R=L}}while(0);Q=c[P+12>>2]|0;if((Q|0)==0){O=R;break}else{L=R;P=Q}}}P=c[K+124>>2]|0;if((P|0)==0){N=O;break}else{V=O;W=P}while(1){P=W+4|0;L=c[P>>2]|0;do{if((a[L+60|0]|0)==0){Q=c[W>>2]|0;M=Q+4|0;U=b[M>>1]|0;if((U&32)==0){X=V;break}T=c[w>>2]|0;c[w>>2]=T+1;c[(c[y>>2]|0)+(T<<2)>>2]=L;a[(c[P>>2]|0)+60|0]=1;if(!((U&1)==0)){X=V;break}c[p+(V<<2)>>2]=Q;b[M>>1]=U|1;X=V+1|0}else{X=V}}while(0);P=c[W+12>>2]|0;if((P|0)==0){N=X;break}else{V=X;W=P}}}}while(0);if((N|0)<=0){break}H=N;I=c[u>>2]|0}Df(h,j,e,A,(a[B]|0)!=0);g[n>>2]=+g[C>>2]+ +g[n>>2];g[o>>2]=+g[D>>2]+ +g[o>>2];g[m>>2]=+g[E>>2]+ +g[m>>2];I=c[u>>2]|0;if((I|0)<=0){break}H=c[x>>2]|0;K=0;do{J=c[H+(K<<2)>>2]|0;if((c[J>>2]|0)==0){P=J+4|0;b[P>>1]=b[P>>1]&-2}K=K+1|0;}while((K|0)<(I|0))}}while(0);F=c[F+112>>2]|0;}while((F|0)!=0)}qn(r,t);$m(k);t=c[l>>2]|0;if((t|0)!=0){l=t;do{do{if(!((b[l+4>>1]&1)==0)){if((c[l>>2]|0)==0){break}_j(l)}}while(0);l=c[l+112>>2]|0;}while((l|0)!=0)}ik(q);g[d+103032>>2]=+bn(k);Cf(h);i=f;return}function Qj(d,e){d=d|0;e=e|0;var f=0,h=0,j=0,l=0,m=0,n=0,o=0,p=0,q=0,r=0,s=0,t=0,u=0,v=0,w=0,x=0,y=0,z=0,A=0,B=0,C=0,D=0,E=0,F=0,G=0,H=0,I=0,J=0,K=0,L=0,M=0,N=0,O=0,P=0,Q=0,R=0,S=0,V=0,W=0,X=0,Y=0,Z=0,_=0,$=0,aa=0,ba=0,ca=0,da=0.0,ea=0,fa=0,ga=0,ha=0,ia=0.0,ja=0,ka=0,la=0,ma=0,na=0,oa=0,pa=0,qa=0,ra=0,sa=0.0,ta=0.0,ua=0.0,va=0,wa=0.0,xa=0,ya=0.0,za=0.0,Aa=0.0,Ba=0.0,Ca=0.0,Da=0,Ea=0,Fa=0,Ga=0,Ha=0,Ia=0,Ja=0,Ka=0,La=0,Ma=0,Na=0,Oa=0,Pa=0,Qa=0,Ra=0,Sa=0,Ta=0,Ua=0;f=i;i=i+360|0;h=f|0;j=f+56|0;l=f+192|0;m=f+200|0;n=f+240|0;o=f+280|0;p=f+288|0;q=f+328|0;r=d+102880|0;s=d+102952|0;Bf(h,64,32,0,d+76|0,c[s>>2]|0);t=d+103007|0;do{if((a[t]|0)==0){u=d+102940|0}else{v=c[d+102960>>2]|0;if((v|0)!=0){w=v;do{v=w+4|0;b[v>>1]=b[v>>1]&-2;g[w+76>>2]=0.0;w=c[w+112>>2]|0;}while((w|0)!=0)}w=d+102940|0;v=c[w>>2]|0;if((v|0)==0){u=w;break}else{x=v}while(1){v=x+4|0;c[v>>2]=c[v>>2]&-34;c[x+128>>2]=0;g[x+132>>2]=1.0;v=c[x+12>>2]|0;if((v|0)==0){u=w;break}else{x=v}}}}while(0);x=m;m=n;n=h+28|0;w=h+36|0;v=h+32|0;y=h+8|0;z=h+12|0;A=o|0;B=o+4|0;C=h+40|0;D=h+44|0;E=p;p=e|0;F=q|0;G=q+4|0;H=q+8|0;I=q+16|0;J=e+12|0;K=q+12|0;L=e+20|0;e=q+20|0;M=q+24|0;N=d+103006|0;d=j+16|0;O=j+20|0;P=j+24|0;Q=j+44|0;R=j+48|0;S=j+52|0;V=j|0;W=j+28|0;X=j+56|0;Y=j+92|0;Z=j+128|0;_=l|0;$=l+4|0;while(1){aa=c[u>>2]|0;if((aa|0)==0){ba=1;ca=83;break}else{da=1.0;ea=0;fa=aa}while(1){aa=fa+4|0;ga=c[aa>>2]|0;do{if((ga&4|0)==0){ha=ea;ia=da}else{if((c[fa+128>>2]|0)>8){ha=ea;ia=da;break}if((ga&32|0)==0){ja=c[fa+48>>2]|0;ka=c[fa+52>>2]|0;if((a[ja+38|0]|0)!=0){ha=ea;ia=da;break}if((a[ka+38|0]|0)!=0){ha=ea;ia=da;break}la=c[ja+8>>2]|0;ma=c[ka+8>>2]|0;na=c[la>>2]|0;oa=c[ma>>2]|0;pa=b[la+4>>1]|0;qa=b[ma+4>>1]|0;if(((pa&2)==0|(na|0)==0)&((qa&2)==0|(oa|0)==0)){ha=ea;ia=da;break}if((pa&8)==0){ra=(na|0)!=2|0}else{ra=1}if((qa&8)==0){if((ra|0)==0&(oa|0)==2){ha=ea;ia=da;break}}oa=la+44|0;qa=la+76|0;sa=+g[qa>>2];na=ma+44|0;pa=ma+76|0;ta=+g[pa>>2];do{if(sa<ta){ua=(ta-sa)/(1.0-sa);va=la+52|0;wa=+g[va>>2];xa=la+56|0;ya=+g[xa>>2];za=ua*(+g[la+64>>2]-ya);g[va>>2]=wa+ua*(+g[la+60>>2]-wa);g[xa>>2]=ya+za;xa=la+68|0;za=+g[xa>>2];g[xa>>2]=za+ua*(+g[la+72>>2]-za);g[qa>>2]=ta;Aa=ta}else{if(!(ta<sa)){Aa=sa;break}za=(sa-ta)/(1.0-ta);xa=ma+52|0;ua=+g[xa>>2];va=ma+56|0;ya=+g[va>>2];wa=za*(+g[ma+64>>2]-ya);g[xa>>2]=ua+za*(+g[ma+60>>2]-ua);g[va>>2]=ya+wa;va=ma+68|0;wa=+g[va>>2];g[va>>2]=wa+za*(+g[ma+72>>2]-wa);g[pa>>2]=sa;Aa=sa}}while(0);pa=c[fa+56>>2]|0;ma=c[fa+60>>2]|0;c[d>>2]=0;c[O>>2]=0;g[P>>2]=0.0;c[Q>>2]=0;c[R>>2]=0;g[S>>2]=0.0;qe(V,c[ja+12>>2]|0,pa);qe(W,c[ka+12>>2]|0,ma);On(X|0,oa|0,36)|0;On(Y|0,na|0,36)|0;g[Z>>2]=1.0;me(l,j);if((c[_>>2]|0)==3){sa=Aa+(1.0-Aa)*+g[$>>2];Ba=sa<1.0?sa:1.0}else{Ba=1.0}g[fa+132>>2]=Ba;c[aa>>2]=c[aa>>2]|32;Ca=Ba}else{Ca=+g[fa+132>>2]}if(!(Ca<da)){ha=ea;ia=da;break}ha=fa;ia=Ca}}while(0);aa=c[fa+12>>2]|0;if((aa|0)==0){break}else{da=ia;ea=ha;fa=aa}}if((ha|0)==0|ia>.9999988079071045){ba=1;ca=83;break}aa=c[(c[ha+48>>2]|0)+8>>2]|0;ga=c[(c[ha+52>>2]|0)+8>>2]|0;ma=aa+44|0;On(x|0,ma|0,36)|0;pa=ga+44|0;On(m|0,pa|0,36)|0;qa=aa+76|0;sa=+g[qa>>2];ta=(ia-sa)/(1.0-sa);la=aa+60|0;va=aa+52|0;sa=+g[va>>2];xa=aa+64|0;Da=aa+56|0;wa=+g[Da>>2];za=ta*(+g[xa>>2]-wa);g[va>>2]=sa+ta*(+g[la>>2]-sa);g[Da>>2]=wa+za;Da=aa+72|0;va=aa+68|0;za=+g[va>>2];wa=za+ta*(+g[Da>>2]-za);g[va>>2]=wa;g[qa>>2]=ia;qa=aa+52|0;va=aa+60|0;Ea=c[qa>>2]|0;Fa=c[qa+4>>2]|0;c[va>>2]=Ea;c[va+4>>2]=Fa;g[Da>>2]=wa;za=+U(wa);va=aa+20|0;g[va>>2]=za;ta=+T(wa);qa=aa+24|0;g[qa>>2]=ta;Ga=aa+44|0;wa=+g[Ga>>2];Ha=aa+48|0;sa=+g[Ha>>2];ya=(c[k>>2]=Ea,+g[k>>2])-(ta*wa-za*sa);ua=(c[k>>2]=Fa,+g[k>>2])-(za*wa+ta*sa);Fa=aa+12|0;sa=+ya;ya=+ua;g[Fa>>2]=sa;g[Fa+4>>2]=ya;Ea=ga+76|0;ya=+g[Ea>>2];sa=(ia-ya)/(1.0-ya);Ia=ga+60|0;Ja=ga+52|0;ya=+g[Ja>>2];Ka=ga+64|0;La=ga+56|0;ua=+g[La>>2];ta=sa*(+g[Ka>>2]-ua);g[Ja>>2]=ya+sa*(+g[Ia>>2]-ya);g[La>>2]=ua+ta;La=ga+72|0;Ja=ga+68|0;ta=+g[Ja>>2];ua=ta+sa*(+g[La>>2]-ta);g[Ja>>2]=ua;g[Ea>>2]=ia;Ea=ga+52|0;Ja=ga+60|0;Ma=c[Ea>>2]|0;Na=c[Ea+4>>2]|0;c[Ja>>2]=Ma;c[Ja+4>>2]=Na;g[La>>2]=ua;ta=+U(ua);Ja=ga+20|0;g[Ja>>2]=ta;sa=+T(ua);Ea=ga+24|0;g[Ea>>2]=sa;Oa=ga+44|0;ua=+g[Oa>>2];Pa=ga+48|0;ya=+g[Pa>>2];wa=(c[k>>2]=Ma,+g[k>>2])-(sa*ua-ta*ya);za=(c[k>>2]=Na,+g[k>>2])-(ta*ua+sa*ya);Na=ga+12|0;ya=+wa;wa=+za;g[Na>>2]=ya;g[Na+4>>2]=wa;Mi(ha,c[s>>2]|0);Ma=ha+4|0;Qa=c[Ma>>2]|0;c[Ma>>2]=Qa&-33;Ra=ha+128|0;c[Ra>>2]=(c[Ra>>2]|0)+1;if((Qa&6|0)!=6){c[Ma>>2]=Qa&-37;On(ma|0,x|0,36)|0;On(pa|0,m|0,36)|0;wa=+g[Da>>2];ya=+U(wa);g[va>>2]=ya;za=+T(wa);g[qa>>2]=za;wa=+g[Ga>>2];sa=+g[Ha>>2];ua=+(+g[la>>2]-(za*wa-ya*sa));ta=+(+g[xa>>2]-(ya*wa+za*sa));g[Fa>>2]=ua;g[Fa+4>>2]=ta;ta=+g[La>>2];ua=+U(ta);g[Ja>>2]=ua;sa=+T(ta);g[Ea>>2]=sa;ta=+g[Oa>>2];za=+g[Pa>>2];wa=+(+g[Ia>>2]-(sa*ta-ua*za));ya=+(+g[Ka>>2]-(ua*ta+sa*za));g[Na>>2]=wa;g[Na+4>>2]=ya;continue}Na=aa+4|0;Ka=b[Na>>1]|0;if((Ka&2)==0){Ia=Ka|2;b[Na>>1]=Ia;g[aa+160>>2]=0.0;Sa=Ia}else{Sa=Ka}Ka=ga+4|0;Ia=b[Ka>>1]|0;if((Ia&2)==0){b[Ka>>1]=Ia|2;g[ga+160>>2]=0.0;Ta=b[Na>>1]|0}else{Ta=Sa}c[n>>2]=0;c[w>>2]=0;c[v>>2]=0;Ia=aa+8|0;c[Ia>>2]=0;Pa=c[n>>2]|0;c[(c[y>>2]|0)+(Pa<<2)>>2]=aa;Oa=Pa+1|0;c[n>>2]=Oa;Pa=ga+8|0;c[Pa>>2]=Oa;Oa=c[n>>2]|0;c[(c[y>>2]|0)+(Oa<<2)>>2]=ga;c[n>>2]=Oa+1;Oa=c[w>>2]|0;c[w>>2]=Oa+1;c[(c[z>>2]|0)+(Oa<<2)>>2]=ha;b[Na>>1]=Ta|1;b[Ka>>1]=b[Ka>>1]|1;c[Ma>>2]=c[Ma>>2]|1;c[A>>2]=aa;c[B>>2]=ga;ga=1;Ma=aa;while(1){a:do{if((c[Ma>>2]|0)==2){aa=c[Ma+128>>2]|0;if((aa|0)==0){break}Ka=Ma+4|0;Na=aa;do{if((c[n>>2]|0)==(c[C>>2]|0)){break a}if((c[w>>2]|0)==(c[D>>2]|0)){break a}aa=c[Na+4>>2]|0;Oa=aa+4|0;b:do{if((c[Oa>>2]&1|0)==0){Ea=c[Na>>2]|0;Ja=Ea|0;do{if((c[Ja>>2]|0)==2){if(!((b[Ka>>1]&8)==0)){break}if((b[Ea+4>>1]&8)==0){break b}}}while(0);if((a[(c[aa+48>>2]|0)+38|0]|0)!=0){break}if((a[(c[aa+52>>2]|0)+38|0]|0)!=0){break}La=Ea+44|0;On(E|0,La|0,36)|0;Fa=Ea+4|0;if((b[Fa>>1]&1)==0){xa=Ea+76|0;ya=+g[xa>>2];wa=(ia-ya)/(1.0-ya);la=Ea+52|0;ya=+g[la>>2];Ha=Ea+56|0;za=+g[Ha>>2];sa=wa*(+g[Ea+64>>2]-za);g[la>>2]=ya+wa*(+g[Ea+60>>2]-ya);g[Ha>>2]=za+sa;Ha=Ea+72|0;la=Ea+68|0;sa=+g[la>>2];za=sa+wa*(+g[Ha>>2]-sa);g[la>>2]=za;g[xa>>2]=ia;xa=Ea+52|0;la=Ea+60|0;Ga=c[xa>>2]|0;qa=c[xa+4>>2]|0;c[la>>2]=Ga;c[la+4>>2]=qa;g[Ha>>2]=za;sa=+U(za);g[Ea+20>>2]=sa;wa=+T(za);g[Ea+24>>2]=wa;za=+g[Ea+44>>2];ya=+g[Ea+48>>2];ta=(c[k>>2]=Ga,+g[k>>2])-(wa*za-sa*ya);ua=(c[k>>2]=qa,+g[k>>2])-(sa*za+wa*ya);qa=Ea+12|0;ya=+ta;ta=+ua;g[qa>>2]=ya;g[qa+4>>2]=ta}Mi(aa,c[s>>2]|0);qa=c[Oa>>2]|0;if((qa&4|0)==0){On(La|0,E|0,36)|0;ta=+g[Ea+72>>2];ya=+U(ta);g[Ea+20>>2]=ya;ua=+T(ta);g[Ea+24>>2]=ua;ta=+g[Ea+44>>2];wa=+g[Ea+48>>2];Ga=Ea+12|0;za=+(+g[Ea+60>>2]-(ua*ta-ya*wa));sa=+(+g[Ea+64>>2]-(ya*ta+ua*wa));g[Ga>>2]=za;g[Ga+4>>2]=sa;break}if((qa&2|0)==0){On(La|0,E|0,36)|0;sa=+g[Ea+72>>2];za=+U(sa);g[Ea+20>>2]=za;wa=+T(sa);g[Ea+24>>2]=wa;sa=+g[Ea+44>>2];ua=+g[Ea+48>>2];La=Ea+12|0;ta=+(+g[Ea+60>>2]-(wa*sa-za*ua));ya=+(+g[Ea+64>>2]-(za*sa+wa*ua));g[La>>2]=ta;g[La+4>>2]=ya;break}c[Oa>>2]=qa|1;qa=c[w>>2]|0;c[w>>2]=qa+1;c[(c[z>>2]|0)+(qa<<2)>>2]=aa;qa=b[Fa>>1]|0;if(!((qa&1)==0)){break}b[Fa>>1]=qa|1;do{if((c[Ja>>2]|0)!=0){if(!((qa&2)==0)){break}b[Fa>>1]=qa|3;g[Ea+160>>2]=0.0}}while(0);c[Ea+8>>2]=c[n>>2];qa=c[n>>2]|0;c[(c[y>>2]|0)+(qa<<2)>>2]=Ea;c[n>>2]=qa+1}}while(0);Na=c[Na+12>>2]|0;}while((Na|0)!=0)}}while(0);if((ga|0)>=2){break}Na=c[o+(ga<<2)>>2]|0;ga=ga+1|0;Ma=Na}ya=(1.0-ia)*+g[p>>2];g[F>>2]=ya;g[G>>2]=1.0/ya;g[H>>2]=1.0;c[I>>2]=20;c[K>>2]=c[J>>2];c[e>>2]=c[L>>2];a[M]=0;Ef(h,q,c[Ia>>2]|0,c[Pa>>2]|0);Ma=c[n>>2]|0;if((Ma|0)>0){ga=0;Na=Ma;while(1){Ma=c[(c[y>>2]|0)+(ga<<2)>>2]|0;Ka=Ma+4|0;b[Ka>>1]=b[Ka>>1]&-2;if((c[Ma>>2]|0)==2){_j(Ma);Ka=c[Ma+128>>2]|0;if((Ka|0)!=0){Ma=Ka;do{Ka=(c[Ma+4>>2]|0)+4|0;c[Ka>>2]=c[Ka>>2]&-34;Ma=c[Ma+12>>2]|0;}while((Ma|0)!=0)}Ua=c[n>>2]|0}else{Ua=Na}Ma=ga+1|0;if((Ma|0)<(Ua|0)){ga=Ma;Na=Ua}else{break}}}ik(r);if((a[N]|0)!=0){ba=0;ca=83;break}}if((ca|0)==83){a[t]=ba;Cf(h);i=f;return}}function Rj(b,d,e,f,h){b=b|0;d=+d;e=e|0;f=f|0;h=h|0;var j=0,k=0,l=0,m=0,n=0,o=0,p=0,q=0,r=0,s=0,t=0.0,u=0,v=0.0,w=0,x=0;j=i;i=i+64|0;k=j|0;l=j+8|0;m=j+40|0;n=j+48|0;o=j+56|0;$m(k);p=b+102876|0;q=c[p>>2]|0;if((q&1|0)==0){r=q}else{ik(b+102880|0);q=c[p>>2]&-2;c[p>>2]=q;r=q}c[p>>2]=r|2;r=l|0;g[r>>2]=d;c[l+12>>2]=e;c[l+16>>2]=f;c[l+20>>2]=h;h=d>0.0;if(h){g[l+4>>2]=1.0/d}else{g[l+4>>2]=0.0}f=b+103e3|0;g[l+8>>2]=+g[f>>2]*d;a[l+24|0]=a[b+103004|0]|0;$m(m);hk(b+102880|0);g[b+103012>>2]=+bn(m);if(!((a[b+103007|0]|0)==0|h^1)){$m(n);h=c[b+102968>>2]|0;if((h|0)!=0){m=h;do{ll(m,l);m=c[m+408>>2]|0;}while((m|0)!=0)}Pj(b,l);g[b+103016>>2]=+bn(n)}do{if((a[b+103005|0]|0)==0){s=13}else{d=+g[r>>2];if(!(d>0.0)){t=d;break}$m(o);Qj(b,l);g[b+103036>>2]=+bn(o);s=13}}while(0);if((s|0)==13){t=+g[r>>2]}if(t>0.0){g[f>>2]=+g[l+4>>2]}l=c[p>>2]|0;if((l&4|0)==0){u=l&-3;c[p>>2]=u;v=+bn(k);w=b+103008|0;g[w>>2]=v;i=j;return}f=c[b+102960>>2]|0;if((f|0)==0){u=l&-3;c[p>>2]=u;v=+bn(k);w=b+103008|0;g[w>>2]=v;i=j;return}else{x=f}do{g[x+92>>2]=0.0;g[x+96>>2]=0.0;g[x+100>>2]=0.0;x=c[x+112>>2]|0;}while((x|0)!=0);u=l&-3;c[p>>2]=u;v=+bn(k);w=b+103008|0;g[w>>2]=v;i=j;return}function Sj(a,b,d){a=a|0;b=b|0;d=d|0;var e=0,f=0,g=0;e=i;i=i+8|0;f=e|0;g=a+102880|0;c[f>>2]=g;c[f+4>>2]=b;Vj(g|0,f,d);f=c[a+102968>>2]|0;if((f|0)==0){i=e;return}a=b;g=f;do{if(tb[c[(c[a>>2]|0)+16>>2]&63](b,g)|0){Cl(g,b,d)}g=c[g+408>>2]|0;}while((g|0)!=0);i=e;return}function Tj(a,b,d,e){a=a|0;b=b|0;d=d|0;e=e|0;var f=0,h=0,j=0,k=0,l=0,m=0,n=0;f=i;i=i+32|0;h=f|0;j=f+8|0;k=a+102880|0;c[h>>2]=k;c[h+4>>2]=b;g[j+16>>2]=1.0;l=d;m=j;n=c[l+4>>2]|0;c[m>>2]=c[l>>2];c[m+4>>2]=n;n=e;m=j+8|0;l=c[n+4>>2]|0;c[m>>2]=c[n>>2];c[m+4>>2]=l;Uj(k|0,h,j);j=c[a+102968>>2]|0;if((j|0)==0){i=f;return}a=b;h=j;do{if(tb[c[(c[a>>2]|0)+16>>2]&63](b,h)|0){Dl(h,b,d,e)}h=c[h+408>>2]|0;}while((h|0)!=0);i=f;return}function Uj(a,b,d){a=a|0;b=b|0;d=d|0;var e=0,f=0,h=0,j=0,k=0,l=0,m=0.0,n=0.0,o=0,p=0.0,q=0.0,r=0.0,s=0.0,t=0.0,u=0.0,v=0.0,w=0.0,x=0.0,y=0,z=0,A=0,B=0,C=0,D=0,E=0,F=0,G=0,H=0,I=0,J=0,K=0,L=0,M=0,N=0,O=0,P=0,Q=0,S=0.0,T=0.0,U=0.0,V=0.0,W=0.0,X=0,Y=0,Z=0,_=0,$=0,aa=0,ba=0,ca=0,da=0.0,ea=0.0,fa=0.0,ga=0.0,ha=0.0,ia=0.0,ja=0.0,ka=0.0,la=0.0,ma=0.0,na=0.0,oa=0,pa=0,qa=0,ra=0,sa=0,ta=0,ua=0.0;e=i;i=i+1088|0;f=e|0;h=e+16|0;j=e+24|0;k=e+1064|0;l=d;m=+g[l>>2];n=+g[l+4>>2];o=d+8|0;p=+g[o>>2];q=p-m;p=+g[o+4>>2]-n;r=+R(q*q+p*p);if(r<1.1920928955078125e-7){s=q;t=p}else{u=1.0/r;s=q*u;t=p*u}u=t*-1.0;if(u>0.0){v=u}else{v=-0.0-u}if(s>0.0){w=s}else{w=-0.0-s}t=+g[d+16>>2];r=m+q*t;x=n+p*t;d=j+4|0;y=j|0;c[y>>2]=d;z=j+1028|0;c[z>>2]=0;A=j+1032|0;c[A>>2]=256;j=c[y>>2]|0;c[j+(c[z>>2]<<2)>>2]=c[a>>2];B=(c[z>>2]|0)+1|0;c[z>>2]=B;a:do{if((B|0)>0){C=a+4|0;D=k;E=k+8|0;F=k+16|0;G=b|0;H=f+8|0;I=k|0;J=k+4|0;K=k+8|0;L=k+12|0;M=h|0;N=h+4|0;O=b+4|0;P=f|0;Q=B;S=n<x?n:x;T=m<r?m:r;U=n>x?n:x;V=m>r?m:r;W=t;X=j;while(1){Y=Q;Z=X;while(1){_=Y-1|0;c[z>>2]=_;$=c[Z+(_<<2)>>2]|0;if(($|0)==-1){aa=_;ba=Z}else{ca=c[C>>2]|0;da=+g[ca+($*36|0)+8>>2];ea=+g[ca+($*36|0)+12>>2];fa=+g[ca+($*36|0)>>2];ga=+g[ca+($*36|0)+4>>2];if(T-da>0.0|S-ea>0.0|fa-V>0.0|ga-U>0.0){ha=W;ia=V;ja=U;ka=T;la=S;break}ma=u*(m-(da+fa)*.5)+s*(n-(ea+ga)*.5);if(ma>0.0){na=ma}else{na=-0.0-ma}if(na-(v*(da-fa)*.5+w*(ea-ga)*.5)>0.0){ha=W;ia=V;ja=U;ka=T;la=S;break}oa=ca+($*36|0)+24|0;if((c[oa>>2]|0)==-1){pa=24;break}do{if((_|0)==(c[A>>2]|0)){c[A>>2]=_<<1;qa=en(_<<3)|0;c[y>>2]=qa;ra=Z;On(qa|0,ra|0,c[z>>2]<<2)|0;if((Z|0)==(d|0)){break}fn(ra)}}while(0);_=c[y>>2]|0;c[_+(c[z>>2]<<2)>>2]=c[oa>>2];ra=(c[z>>2]|0)+1|0;c[z>>2]=ra;qa=ca+($*36|0)+28|0;do{if((ra|0)==(c[A>>2]|0)){c[A>>2]=ra<<1;sa=en(ra<<3)|0;c[y>>2]=sa;ta=_;On(sa|0,ta|0,c[z>>2]<<2)|0;if((_|0)==(d|0)){break}fn(ta)}}while(0);_=c[y>>2]|0;c[_+(c[z>>2]<<2)>>2]=c[qa>>2];ra=(c[z>>2]|0)+1|0;c[z>>2]=ra;aa=ra;ba=_}if((aa|0)>0){Y=aa;Z=ba}else{break a}}do{if((pa|0)==24){pa=0;Z=c[l+4>>2]|0;c[D>>2]=c[l>>2];c[D+4>>2]=Z;Z=c[o+4>>2]|0;c[E>>2]=c[o>>2];c[E+4>>2]=Z;g[F>>2]=W;Z=c[(c[(c[G>>2]|0)+4>>2]|0)+($*36|0)+16>>2]|0;Y=c[Z+16>>2]|0;_=c[Y+12>>2]|0;if(ub[c[(c[_>>2]|0)+24>>2]&31](_,f,k,(c[Y+8>>2]|0)+12|0,c[Z+20>>2]|0)|0){ga=+g[H>>2];ea=1.0-ga;fa=ea*+g[J>>2]+ga*+g[L>>2];g[M>>2]=+g[I>>2]*ea+ga*+g[K>>2];g[N>>2]=fa;Z=c[O>>2]|0;ua=+vb[c[(c[Z>>2]|0)+8>>2]&3](Z,Y,h,P,ga)}else{ua=+g[F>>2]}if(ua==0.0){break a}if(!(ua>0.0)){ha=W;ia=V;ja=U;ka=T;la=S;break}ga=m+q*ua;fa=n+p*ua;ha=ua;ia=m>ga?m:ga;ja=n>fa?n:fa;ka=m<ga?m:ga;la=n<fa?n:fa}}while(0);Y=c[z>>2]|0;if((Y|0)<=0){break a}Q=Y;S=la;T=ka;U=ja;V=ia;W=ha;X=c[y>>2]|0}}}while(0);z=c[y>>2]|0;if((z|0)==(d|0)){i=e;return}fn(z);c[y>>2]=0;i=e;return}function Vj(a,b,d){a=a|0;b=b|0;d=d|0;var e=0,f=0,h=0,j=0,k=0,l=0,m=0,n=0,o=0,p=0,q=0,r=0,s=0,t=0,u=0,v=0,w=0,x=0,y=0,z=0,A=0,B=0,C=0,D=0,E=0,F=0;e=i;i=i+1040|0;f=e|0;h=f+4|0;j=f|0;c[j>>2]=h;k=f+1028|0;c[k>>2]=0;l=f+1032|0;c[l>>2]=256;f=c[j>>2]|0;c[f+(c[k>>2]<<2)>>2]=c[a>>2];m=(c[k>>2]|0)+1|0;c[k>>2]=m;a:do{if((m|0)>0){n=a+4|0;o=d|0;p=d+4|0;q=d+8|0;r=d+12|0;s=b|0;t=b+4|0;u=m;v=f;while(1){w=u-1|0;c[k>>2]=w;x=c[v+(w<<2)>>2]|0;do{if((x|0)==-1){y=w}else{z=c[n>>2]|0;if(+g[o>>2]- +g[z+(x*36|0)+8>>2]>0.0|+g[p>>2]- +g[z+(x*36|0)+12>>2]>0.0|+g[z+(x*36|0)>>2]- +g[q>>2]>0.0|+g[z+(x*36|0)+4>>2]- +g[r>>2]>0.0){y=w;break}A=z+(x*36|0)+24|0;if((c[A>>2]|0)==-1){B=c[t>>2]|0;if(!(tb[c[(c[B>>2]|0)+8>>2]&63](B,c[(c[(c[(c[s>>2]|0)+4>>2]|0)+(x*36|0)+16>>2]|0)+16>>2]|0)|0)){break a}y=c[k>>2]|0;break}do{if((w|0)==(c[l>>2]|0)){c[l>>2]=w<<1;B=en(w<<3)|0;c[j>>2]=B;C=v;On(B|0,C|0,c[k>>2]<<2)|0;if((v|0)==(h|0)){break}fn(C)}}while(0);C=c[j>>2]|0;c[C+(c[k>>2]<<2)>>2]=c[A>>2];B=(c[k>>2]|0)+1|0;c[k>>2]=B;D=z+(x*36|0)+28|0;do{if((B|0)==(c[l>>2]|0)){c[l>>2]=B<<1;E=en(B<<3)|0;c[j>>2]=E;F=C;On(E|0,F|0,c[k>>2]<<2)|0;if((C|0)==(h|0)){break}fn(F)}}while(0);c[(c[j>>2]|0)+(c[k>>2]<<2)>>2]=c[D>>2];C=(c[k>>2]|0)+1|0;c[k>>2]=C;y=C}}while(0);if((y|0)<=0){break a}u=y;v=c[j>>2]|0}}}while(0);y=c[j>>2]|0;if((y|0)==(h|0)){i=e;return}fn(y);c[j>>2]=0;i=e;return}function Wj(d,e,f){d=d|0;e=e|0;f=f|0;var h=0,i=0,j=0,k=0,l=0,m=0,n=0,o=0.0,p=0,q=0,r=0,s=0,t=0,u=0,v=0;h=d+12|0;i=d+4|0;b[i>>1]=0;if((a[e+39|0]|0)==0){j=0}else{b[i>>1]=8;j=8}if((a[e+38|0]|0)==0){k=j}else{l=j|16;b[i>>1]=l;k=l}if((a[e+36|0]|0)==0){m=k}else{l=k|4;b[i>>1]=l;m=l}if((a[e+37|0]|0)==0){n=m}else{l=m|2;b[i>>1]=l;n=l}if((a[e+40|0]|0)!=0){b[i>>1]=n|32}c[d+104>>2]=f;f=e+4|0;n=h;i=c[f>>2]|0;l=c[f+4>>2]|0;c[n>>2]=i;c[n+4>>2]=l;n=e+12|0;o=+g[n>>2];g[d+20>>2]=+U(o);g[d+24>>2]=+T(o);f=d+28|0;m=h;c[f>>2]=c[m>>2];c[f+4>>2]=c[m+4>>2];c[f+8>>2]=c[m+8>>2];c[f+12>>2]=c[m+12>>2];g[d+44>>2]=0.0;g[d+48>>2]=0.0;m=d+52|0;c[m>>2]=i;c[m+4>>2]=l;m=d+60|0;c[m>>2]=i;c[m+4>>2]=l;g[d+68>>2]=+g[n>>2];g[d+72>>2]=+g[n>>2];g[d+76>>2]=0.0;c[d+124>>2]=0;c[d+128>>2]=0;c[d+108>>2]=0;c[d+112>>2]=0;n=e+16|0;l=d+80|0;m=c[n+4>>2]|0;c[l>>2]=c[n>>2];c[l+4>>2]=m;g[d+88>>2]=+g[e+24>>2];g[d+148>>2]=+g[e+28>>2];g[d+152>>2]=+g[e+32>>2];g[d+156>>2]=+g[e+48>>2];g[d+92>>2]=0.0;g[d+96>>2]=0.0;g[d+100>>2]=0.0;g[d+160>>2]=0.0;m=c[e>>2]|0;c[d>>2]=m;l=d+132|0;if((m|0)==2){g[l>>2]=1.0;g[d+136>>2]=1.0;p=d+140|0;g[p>>2]=0.0;q=d+144|0;g[q>>2]=0.0;r=e+44|0;s=c[r>>2]|0;t=d+164|0;c[t>>2]=s;u=d+116|0;c[u>>2]=0;v=d+120|0;c[v>>2]=0;return}else{g[l>>2]=0.0;g[d+136>>2]=0.0;p=d+140|0;g[p>>2]=0.0;q=d+144|0;g[q>>2]=0.0;r=e+44|0;s=c[r>>2]|0;t=d+164|0;c[t>>2]=s;u=d+116|0;c[u>>2]=0;v=d+120|0;c[v>>2]=0;return}}function Xj(a){a=a|0;return}function Yj(a,d){a=a|0;d=d|0;var e=0,f=0,h=0,j=0,l=0.0,m=0,n=0,o=0,p=0,q=0.0,r=0.0,s=0.0,t=0.0,u=0.0,v=0;e=i;i=i+16|0;f=e|0;h=a+104|0;if((c[(c[h>>2]|0)+102876>>2]&2|0)!=0){i=e;return}j=a|0;if((c[j>>2]|0)==(d|0)){i=e;return}c[j>>2]=d;Zj(a);do{if((c[j>>2]|0)==0){g[a+80>>2]=0.0;g[a+84>>2]=0.0;g[a+88>>2]=0.0;l=+g[a+72>>2];g[a+68>>2]=l;d=a+60|0;m=a+52|0;n=c[d>>2]|0;o=c[d+4>>2]|0;c[m>>2]=n;c[m+4>>2]=o;m=f|0;d=f;p=f+8|0;q=+U(l);g[p>>2]=q;r=+T(l);g[p+4>>2]=r;l=+g[a+44>>2];s=+g[a+48>>2];t=(c[k>>2]=n,+g[k>>2])-(r*l-q*s);u=(c[k>>2]=o,+g[k>>2])-(q*l+r*s);s=+t;t=+u;g[m>>2]=s;g[m+4>>2]=t;m=(c[h>>2]|0)+102880|0;o=c[a+116>>2]|0;if((o|0)==0){break}n=a+12|0;p=o;do{Fj(p,m,d,n);p=c[p+4>>2]|0;}while((p|0)!=0)}}while(0);f=a+4|0;j=b[f>>1]|0;if((j&2)==0){b[f>>1]=j|2;g[a+160>>2]=0.0}g[a+92>>2]=0.0;g[a+96>>2]=0.0;g[a+100>>2]=0.0;j=a+128|0;f=c[j>>2]|0;if((f|0)!=0){p=f;while(1){f=c[p+12>>2]|0;gk((c[h>>2]|0)+102880|0,c[p+4>>2]|0);if((f|0)==0){break}else{p=f}}}c[j>>2]=0;j=(c[h>>2]|0)+102880|0;h=c[a+116>>2]|0;if((h|0)==0){i=e;return}else{v=h}do{h=c[v+28>>2]|0;if((h|0)>0){a=v+24|0;p=0;do{ef(j,c[(c[a>>2]|0)+(p*28|0)+24>>2]|0);p=p+1|0;}while((p|0)<(h|0))}v=c[v+4>>2]|0;}while((v|0)!=0);i=e;return}function Zj(a){a=a|0;var d=0,e=0,f=0,h=0,j=0,l=0,m=0,n=0,o=0,p=0,q=0,r=0.0,s=0.0,t=0.0,u=0.0,v=0.0,w=0,x=0,y=0.0,z=0.0,A=0,B=0.0,C=0.0,D=0.0,E=0.0,F=0.0,G=0.0,H=0.0,I=0,J=0.0,K=0.0,L=0.0,M=0.0,N=0.0,O=0.0,P=0.0,Q=0.0;d=i;i=i+16|0;e=d|0;f=a+132|0;h=a+136|0;j=a+140|0;l=a+144|0;m=a+44|0;g[m>>2]=0.0;g[a+48>>2]=0.0;Nn(f|0,0,16)|0;if((c[a>>2]|0)>>>0<2>>>0){n=a+12|0;o=a+52|0;p=c[n>>2]|0;q=c[n+4>>2]|0;c[o>>2]=p;c[o+4>>2]=q;o=a+60|0;c[o>>2]=p;c[o+4>>2]=q;g[a+68>>2]=+g[a+72>>2];i=d;return}q=8600;r=+g[q>>2];s=+g[q+4>>2];q=c[a+116>>2]|0;do{if((q|0)==0){t=0.0;u=r;v=s;w=10}else{o=e|0;p=e+4|0;n=e+8|0;x=e+12|0;y=s;z=r;A=q;B=0.0;C=0.0;while(1){D=+g[A>>2];if(D==0.0){E=z;F=y;G=B;H=C}else{I=c[A+12>>2]|0;rb[c[(c[I>>2]|0)+32>>2]&63](I,e,D);D=+g[o>>2];J=D+ +g[f>>2];g[f>>2]=J;K=z+D*+g[p>>2];L=y+D*+g[n>>2];D=+g[x>>2]+ +g[j>>2];g[j>>2]=D;E=K;F=L;G=J;H=D}I=c[A+4>>2]|0;if((I|0)==0){break}else{y=F;z=E;A=I;B=G;C=H}}if(!(G>0.0)){t=H;u=E;v=F;w=10;break}C=1.0/G;g[h>>2]=C;M=E*C;N=F*C;O=G;P=H}}while(0);if((w|0)==10){g[f>>2]=1.0;g[h>>2]=1.0;M=u;N=v;O=1.0;P=t}do{if(P>0.0){if(!((b[a+4>>1]&16)==0)){w=14;break}t=P-(N*N+M*M)*O;g[j>>2]=t;Q=1.0/t}else{w=14}}while(0);if((w|0)==14){g[j>>2]=0.0;Q=0.0}g[l>>2]=Q;l=a+60|0;Q=+g[l>>2];O=+g[l+4>>2];j=m;P=+M;t=+N;g[j>>2]=P;g[j+4>>2]=t;t=+g[a+24>>2];P=+g[a+20>>2];v=+g[a+12>>2]+(t*M-P*N);u=M*P+t*N+ +g[a+16>>2];j=(g[k>>2]=v,c[k>>2]|0);m=j|0;N=+u;c[l>>2]=m;g[l+4>>2]=N;l=a+52|0;c[l>>2]=m;g[l+4>>2]=N;N=+g[a+88>>2];l=a+80|0;g[l>>2]=+g[l>>2]+(u-O)*(-0.0-N);l=a+84|0;g[l>>2]=N*(v-Q)+ +g[l>>2];i=d;return}function _j(a){a=a|0;var b=0,d=0,e=0,f=0,h=0,j=0.0,k=0.0,l=0.0,m=0.0,n=0.0,o=0.0;b=i;i=i+16|0;d=b|0;e=d|0;f=d;h=d+8|0;j=+g[a+68>>2];k=+U(j);g[h>>2]=k;l=+T(j);g[h+4>>2]=l;j=+g[a+44>>2];m=+g[a+48>>2];n=+(+g[a+52>>2]-(l*j-k*m));o=+(+g[a+56>>2]-(j*k+l*m));g[e>>2]=n;g[e+4>>2]=o;e=(c[a+104>>2]|0)+102880|0;h=c[a+116>>2]|0;if((h|0)==0){i=b;return}d=a+12|0;a=h;do{Fj(a,e,f,d);a=c[a+4>>2]|0;}while((a|0)!=0);i=b;return}function $j(a,d){a=a|0;d=d|0;var e=0,f=0,h=0,i=0,j=0,k=0;e=a+104|0;f=c[e>>2]|0;if((c[f+102876>>2]&2|0)!=0){h=0;return h|0}i=f|0;f=Vm(i,44)|0;if((f|0)==0){j=0}else{k=f;Aj(k);j=k}Bj(j,i,a,d);if(!((b[a+4>>1]&32)==0)){Dj(j,(c[e>>2]|0)+102880|0,a+12|0)}d=a+116|0;c[j+4>>2]=c[d>>2];c[d>>2]=j;d=a+120|0;c[d>>2]=(c[d>>2]|0)+1;c[j+8>>2]=a;if(+g[j>>2]>0.0){Zj(a)}a=(c[e>>2]|0)+102876|0;c[a>>2]=c[a>>2]|1;h=j;return h|0}function ak(a,d){a=a|0;d=d|0;var e=0,f=0,g=0,h=0;e=a+104|0;if((c[(c[e>>2]|0)+102876>>2]&2|0)!=0){return}f=a+116|0;while(1){g=c[f>>2]|0;if((g|0)==0){break}if((g|0)==(d|0)){h=5;break}else{f=g+4|0}}if((h|0)==5){c[f>>2]=c[d+4>>2]}f=c[a+128>>2]|0;if((f|0)!=0){h=f;do{f=c[h+4>>2]|0;h=c[h+12>>2]|0;if((c[f+48>>2]|0)==(d|0)|(c[f+52>>2]|0)==(d|0)){gk((c[e>>2]|0)+102880|0,f)}}while((h|0)!=0)}h=c[e>>2]|0;e=h|0;if(!((b[a+4>>1]&32)==0)){Ej(d,h+102880|0)}Cj(d,e);c[d+8>>2]=0;c[d+4>>2]=0;Wm(e,d,44);d=a+120|0;c[d>>2]=(c[d>>2]|0)-1;Zj(a);return}function bk(a,d){a=a|0;d=d|0;var e=0,f=0,h=0,i=0.0,j=0.0,l=0.0,m=0.0,n=0.0,o=0,p=0.0,q=0.0,r=0.0;if((c[(c[a+104>>2]|0)+102876>>2]&2|0)!=0){return}if((c[a>>2]|0)!=2){return}e=a+136|0;g[e>>2]=0.0;f=a+140|0;g[f>>2]=0.0;h=a+144|0;g[h>>2]=0.0;i=+g[d>>2];j=i>0.0?i:1.0;g[a+132>>2]=j;g[e>>2]=1.0/j;i=+g[d+12>>2];do{if(i>0.0){if(!((b[a+4>>1]&16)==0)){break}l=+g[d+4>>2];m=+g[d+8>>2];n=i-j*(l*l+m*m);g[f>>2]=n;g[h>>2]=1.0/n}}while(0);h=a+60|0;j=+g[h>>2];i=+g[h+4>>2];f=d+4|0;d=a+44|0;e=c[f>>2]|0;o=c[f+4>>2]|0;c[d>>2]=e;c[d+4>>2]=o;n=+g[a+24>>2];m=(c[k>>2]=e,+g[k>>2]);l=+g[a+20>>2];p=(c[k>>2]=o,+g[k>>2]);q=+g[a+12>>2]+(n*m-l*p);r=m*l+n*p+ +g[a+16>>2];o=(g[k>>2]=q,c[k>>2]|0);e=o|0;p=+r;c[h>>2]=e;g[h+4>>2]=p;h=a+52|0;c[h>>2]=e;g[h+4>>2]=p;p=+g[a+88>>2];h=a+80|0;g[h>>2]=+g[h>>2]+(r-i)*(-0.0-p);h=a+84|0;g[h>>2]=p*(q-j)+ +g[h>>2];return}function ck(b,d){b=b|0;d=d|0;var e=0,f=0,g=0,h=0;do{if((c[b>>2]|0)!=2){if((c[d>>2]|0)==2){break}else{e=0}return e|0}}while(0);f=c[b+124>>2]|0;if((f|0)==0){e=1;return e|0}else{g=f}while(1){if((c[g>>2]|0)==(d|0)){if((a[(c[g+4>>2]|0)+61|0]|0)==0){e=0;h=7;break}}f=c[g+12>>2]|0;if((f|0)==0){e=1;h=7;break}else{g=f}}if((h|0)==7){return e|0}return 0}function dk(a,b,d){a=a|0;b=b|0;d=+d;var e=0,f=0,h=0.0,i=0.0,j=0,l=0,m=0,n=0.0,o=0.0,p=0.0,q=0.0,r=0;e=c[a+104>>2]|0;if((c[e+102876>>2]&2|0)!=0){return}f=a+12|0;h=+U(d);g[a+20>>2]=h;i=+T(d);g[a+24>>2]=i;j=b;b=f;l=c[j>>2]|0;m=c[j+4>>2]|0;c[b>>2]=l;c[b+4>>2]=m;b=a+28|0;j=f;c[b>>2]=c[j>>2];c[b+4>>2]=c[j+4>>2];c[b+8>>2]=c[j+8>>2];c[b+12>>2]=c[j+12>>2];n=+g[a+44>>2];o=+g[a+48>>2];p=(c[k>>2]=l,+g[k>>2])+(i*n-h*o);q=n*h+i*o+(c[k>>2]=m,+g[k>>2]);m=a+60|0;l=(g[k>>2]=p,c[k>>2]|0);j=l|0;p=+q;c[m>>2]=j;g[m+4>>2]=p;g[a+72>>2]=d;m=a+52|0;c[m>>2]=j;g[m+4>>2]=p;g[a+68>>2]=d;m=e+102880|0;e=c[a+116>>2]|0;if((e|0)==0){return}else{r=e}do{Fj(r,m,f,f);r=c[r+4>>2]|0;}while((r|0)!=0);return}function ek(a){a=a|0;return}function fk(a){a=a|0;$e(a|0);c[a+60>>2]=0;c[a+64>>2]=0;c[a+68>>2]=56;c[a+72>>2]=48;c[a+76>>2]=0;return}function gk(a,b){a=a|0;b=b|0;var d=0,e=0,f=0,g=0,h=0,i=0,j=0,k=0,l=0,m=0;d=c[(c[b+48>>2]|0)+8>>2]|0;e=c[(c[b+52>>2]|0)+8>>2]|0;f=c[a+72>>2]|0;do{if((f|0)!=0){if((c[b+4>>2]&2|0)==0){break}kb[c[(c[f>>2]|0)+12>>2]&255](f,b)}}while(0);f=b+8|0;g=c[f>>2]|0;h=b+12|0;if((g|0)!=0){c[g+12>>2]=c[h>>2]}g=c[h>>2]|0;if((g|0)!=0){c[g+8>>2]=c[f>>2]}f=a+60|0;if((c[f>>2]|0)==(b|0)){c[f>>2]=c[h>>2]}h=b+24|0;f=c[h>>2]|0;g=b+28|0;if((f|0)!=0){c[f+12>>2]=c[g>>2]}f=c[g>>2]|0;if((f|0)!=0){c[f+8>>2]=c[h>>2]}h=d+128|0;if((b+16|0)==(c[h>>2]|0)){c[h>>2]=c[g>>2]}g=b+40|0;h=c[g>>2]|0;d=b+44|0;if((h|0)!=0){c[h+12>>2]=c[d>>2]}h=c[d>>2]|0;if((h|0)!=0){c[h+8>>2]=c[g>>2]}g=e+128|0;if((b+32|0)!=(c[g>>2]|0)){i=a+76|0;j=c[i>>2]|0;Ki(b,j);k=a+64|0;l=c[k>>2]|0;m=l-1|0;c[k>>2]=m;return}c[g>>2]=c[d>>2];i=a+76|0;j=c[i>>2]|0;Ki(b,j);k=a+64|0;l=c[k>>2]|0;m=l-1|0;c[k>>2]=m;return}function hk(a){a=a|0;var d=0,e=0,f=0,h=0,i=0,j=0,k=0,l=0,m=0,n=0,o=0,p=0,q=0,r=0,s=0,t=0,u=0;d=c[a+60>>2]|0;if((d|0)==0){return}e=a+4|0;f=a+72|0;h=a+68|0;i=d;while(1){d=c[i+48>>2]|0;j=c[i+52>>2]|0;k=c[i+56>>2]|0;l=c[i+60>>2]|0;m=c[d+8>>2]|0;n=c[j+8>>2]|0;o=i+4|0;a:do{if((c[o>>2]&8|0)==0){p=11}else{if(!(ck(n,m)|0)){q=c[i+12>>2]|0;gk(a,i);r=q;break}q=c[h>>2]|0;do{if((q|0)!=0){if(lb[c[(c[q>>2]|0)+8>>2]&31](q,d,j)|0){break}s=c[i+12>>2]|0;gk(a,i);r=s;break a}}while(0);c[o>>2]=c[o>>2]&-9;p=11}}while(0);do{if((p|0)==11){p=0;if((b[m+4>>1]&2)==0){t=0}else{t=(c[m>>2]|0)!=0|0}if((b[n+4>>1]&2)==0){u=1}else{u=(c[n>>2]|0)==0}if((t|0)==0&u){r=c[i+12>>2]|0;break}o=c[(c[d+24>>2]|0)+(k*28|0)+24>>2]|0;q=c[(c[j+24>>2]|0)+(l*28|0)+24>>2]|0;s=c[e>>2]|0;if(+g[s+(q*36|0)>>2]- +g[s+(o*36|0)+8>>2]>0.0|+g[s+(q*36|0)+4>>2]- +g[s+(o*36|0)+12>>2]>0.0|+g[s+(o*36|0)>>2]- +g[s+(q*36|0)+8>>2]>0.0|+g[s+(o*36|0)+4>>2]- +g[s+(q*36|0)+12>>2]>0.0){q=c[i+12>>2]|0;gk(a,i);r=q;break}else{Mi(i,c[f>>2]|0);r=c[i+12>>2]|0;break}}}while(0);if((r|0)==0){break}else{i=r}}return}function ik(a){a=a|0;jk(a|0,a);return}function jk(a,b){a=a|0;b=b|0;var d=0,e=0,f=0,g=0,h=0,j=0,k=0,l=0,m=0,n=0,o=0,p=0,q=0;d=i;i=i+8|0;e=d|0;f=a+52|0;c[f>>2]=0;g=a+40|0;h=c[g>>2]|0;if((h|0)>0){j=a+32|0;k=a+56|0;l=a|0;m=a+4|0;n=0;o=h;while(1){h=c[(c[j>>2]|0)+(n<<2)>>2]|0;c[k>>2]=h;if((h|0)==-1){p=o}else{lk(l,a,(c[m>>2]|0)+(h*36|0)|0);p=c[g>>2]|0}h=n+1|0;if((h|0)<(p|0)){n=h;o=p}else{break}}q=c[f>>2]|0}else{q=0}c[g>>2]=0;g=a+44|0;p=c[g>>2]|0;c[e>>2]=32;nk(p,p+(q<<3)|0,e);if((c[f>>2]|0)<=0){i=d;return}e=a+4|0;a=0;a:while(1){q=c[g>>2]|0;p=q+(a<<3)|0;o=c[e>>2]|0;n=q+(a<<3)+4|0;kk(b,c[o+((c[p>>2]|0)*36|0)+16>>2]|0,c[o+((c[n>>2]|0)*36|0)+16>>2]|0);o=c[f>>2]|0;q=a;while(1){m=q+1|0;if((m|0)>=(o|0)){break a}l=c[g>>2]|0;if((c[l+(m<<3)>>2]|0)!=(c[p>>2]|0)){a=m;continue a}if((c[l+(m<<3)+4>>2]|0)==(c[n>>2]|0)){q=m}else{a=m;continue a}}}i=d;return}function kk(d,e,f){d=d|0;e=e|0;f=f|0;var h=0,i=0,j=0,k=0,l=0,m=0,n=0,o=0,p=0,q=0,r=0,s=0;h=c[e+16>>2]|0;i=c[f+16>>2]|0;j=c[e+20>>2]|0;e=c[f+20>>2]|0;f=c[h+8>>2]|0;k=c[i+8>>2]|0;if((f|0)==(k|0)){return}l=c[k+128>>2]|0;a:do{if((l|0)!=0){m=l;while(1){if((c[m>>2]|0)==(f|0)){n=c[m+4>>2]|0;o=c[n+48>>2]|0;p=c[n+52>>2]|0;q=c[n+56>>2]|0;r=c[n+60>>2]|0;if((o|0)==(h|0)&(p|0)==(i|0)&(q|0)==(j|0)&(r|0)==(e|0)){s=24;break}if((o|0)==(i|0)&(p|0)==(h|0)&(q|0)==(e|0)&(r|0)==(j|0)){s=24;break}}m=c[m+12>>2]|0;if((m|0)==0){break a}}if((s|0)==24){return}}}while(0);if(!(ck(k,f)|0)){return}f=c[d+68>>2]|0;do{if((f|0)!=0){if(lb[c[(c[f>>2]|0)+8>>2]&31](f,h,i)|0){break}return}}while(0);f=Ji(h,j,i,e,c[d+76>>2]|0)|0;if((f|0)==0){return}e=c[f+48>>2]|0;i=c[f+52>>2]|0;j=c[e+8>>2]|0;h=c[i+8>>2]|0;c[f+8>>2]=0;k=d+60|0;c[f+12>>2]=c[k>>2];s=c[k>>2]|0;if((s|0)!=0){c[s+8>>2]=f}c[k>>2]=f;k=f+16|0;c[f+20>>2]=f;c[k>>2]=h;c[f+24>>2]=0;s=j+128|0;c[f+28>>2]=c[s>>2];l=c[s>>2]|0;if((l|0)!=0){c[l+8>>2]=k}c[s>>2]=k;k=f+32|0;c[f+36>>2]=f;c[k>>2]=j;c[f+40>>2]=0;s=h+128|0;c[f+44>>2]=c[s>>2];f=c[s>>2]|0;if((f|0)!=0){c[f+8>>2]=k}c[s>>2]=k;do{if((a[e+38|0]|0)==0){if((a[i+38|0]|0)!=0){break}k=j+4|0;s=b[k>>1]|0;if((s&2)==0){b[k>>1]=s|2;g[j+160>>2]=0.0}s=h+4|0;k=b[s>>1]|0;if(!((k&2)==0)){break}b[s>>1]=k|2;g[h+160>>2]=0.0}}while(0);h=d+64|0;c[h>>2]=(c[h>>2]|0)+1;return}function lk(a,b,d){a=a|0;b=b|0;d=d|0;var e=0,f=0,h=0,j=0,k=0,l=0,m=0,n=0,o=0,p=0,q=0,r=0,s=0,t=0,u=0,v=0,w=0,x=0,y=0,z=0,A=0,B=0,C=0,D=0;e=i;i=i+1040|0;f=e|0;h=f+4|0;j=f|0;c[j>>2]=h;k=f+1028|0;c[k>>2]=0;l=f+1032|0;c[l>>2]=256;f=c[j>>2]|0;c[f+(c[k>>2]<<2)>>2]=c[a>>2];m=(c[k>>2]|0)+1|0;c[k>>2]=m;a:do{if((m|0)>0){n=a+4|0;o=d|0;p=d+4|0;q=d+8|0;r=d+12|0;s=m;t=f;while(1){u=s-1|0;c[k>>2]=u;v=c[t+(u<<2)>>2]|0;do{if((v|0)==-1){w=u}else{x=c[n>>2]|0;if(+g[o>>2]- +g[x+(v*36|0)+8>>2]>0.0|+g[p>>2]- +g[x+(v*36|0)+12>>2]>0.0|+g[x+(v*36|0)>>2]- +g[q>>2]>0.0|+g[x+(v*36|0)+4>>2]- +g[r>>2]>0.0){w=u;break}y=x+(v*36|0)+24|0;if((c[y>>2]|0)==-1){if(!(ff(b,v)|0)){break a}w=c[k>>2]|0;break}do{if((u|0)==(c[l>>2]|0)){c[l>>2]=u<<1;z=en(u<<3)|0;c[j>>2]=z;A=t;On(z|0,A|0,c[k>>2]<<2)|0;if((t|0)==(h|0)){break}fn(A)}}while(0);A=c[j>>2]|0;c[A+(c[k>>2]<<2)>>2]=c[y>>2];z=(c[k>>2]|0)+1|0;c[k>>2]=z;B=x+(v*36|0)+28|0;do{if((z|0)==(c[l>>2]|0)){c[l>>2]=z<<1;C=en(z<<3)|0;c[j>>2]=C;D=A;On(C|0,D|0,c[k>>2]<<2)|0;if((A|0)==(h|0)){break}fn(D)}}while(0);c[(c[j>>2]|0)+(c[k>>2]<<2)>>2]=c[B>>2];A=(c[k>>2]|0)+1|0;c[k>>2]=A;w=A}}while(0);if((w|0)<=0){break a}s=w;t=c[j>>2]|0}}}while(0);w=c[j>>2]|0;if((w|0)==(h|0)){i=e;return}fn(w);c[j>>2]=0;i=e;return}function mk(a,b){a=a|0;b=b|0;var d=0,e=0,f=0;d=c[a>>2]|0;e=c[b>>2]|0;if((d|0)<(e|0)){f=1;return f|0}if((d|0)!=(e|0)){f=0;return f|0}f=(c[a+4>>2]|0)<(c[b+4>>2]|0);return f|0}



function tf(b,d,e,f,h,j){b=b|0;d=d|0;e=e|0;f=f|0;h=h|0;j=j|0;var l=0,m=0,n=0,o=0,p=0,q=0,r=0,s=0,t=0,u=0,v=0,w=0.0,x=0.0,y=0.0,z=0.0,A=0.0,B=0.0,C=0.0,D=0.0,E=0.0,F=0.0,G=0,H=0,I=0,J=0,K=0,L=0,M=0,N=0,O=0,P=0,Q=0,S=0,T=0,U=0.0,V=0.0,W=0.0,X=0.0,Y=0.0,Z=0.0,_=0.0,$=0.0,aa=0.0,ba=0,ca=0.0,da=0.0,ea=0,fa=0,ga=0,ha=0,ia=0,ja=0,ka=0,la=0,ma=0,na=0,oa=0,pa=0,qa=0,ra=0,sa=0,ta=0,ua=0,va=0,wa=0,xa=0,ya=0,za=0,Aa=0,Ba=0,Ca=0,Da=0,Ea=0,Fa=0,Ga=0,Ha=0.0,Ia=0,Ja=0,Ka=0.0;l=i;i=i+40|0;m=l|0;n=l+16|0;o=n|0;p=n;q=i;i=i+56|0;r=i;i=i+24|0;s=i;i=i+24|0;t=s|0;u=s;v=b+132|0;w=+g[f+12>>2];x=+g[j+8>>2];y=+g[f+8>>2];z=+g[j+12>>2];A=w*x-y*z;B=x*y+w*z;z=+A;x=+B;C=+g[j>>2]- +g[f>>2];D=+g[j+4>>2]- +g[f+4>>2];E=w*C+y*D;F=C*(-0.0-y)+w*D;D=+E;w=+F;f=v;g[f>>2]=D;g[f+4>>2]=w;f=b+140|0;g[f>>2]=z;g[f+4>>2]=x;f=b+144|0;x=+g[h+12>>2];j=b+140|0;z=+g[h+16>>2];G=v|0;w=E+(B*x-A*z);v=b+136|0;E=x*A+B*z+F;H=b+148|0;F=+w;z=+E;g[H>>2]=F;g[H+4>>2]=z;H=e+28|0;I=b+156|0;J=c[H>>2]|0;K=c[H+4>>2]|0;c[I>>2]=J;c[I+4>>2]=K;I=e+12|0;H=b+164|0;L=c[I>>2]|0;M=c[I+4>>2]|0;c[H>>2]=L;c[H+4>>2]=M;I=e+20|0;N=b+172|0;O=c[I>>2]|0;P=c[I+4>>2]|0;c[N>>2]=O;c[N+4>>2]=P;I=e+36|0;Q=b+180|0;S=c[I>>2]|0;T=c[I+4>>2]|0;c[Q>>2]=S;c[Q+4>>2]=T;Q=(a[e+44|0]|0)!=0;I=(a[e+45|0]|0)!=0;z=(c[k>>2]=O,+g[k>>2]);F=(c[k>>2]=L,+g[k>>2]);B=z-F;A=(c[k>>2]=P,+g[k>>2]);P=b+168|0;x=(c[k>>2]=M,+g[k>>2]);D=A-x;y=+R(B*B+D*D);C=(c[k>>2]=J,+g[k>>2]);U=(c[k>>2]=K,+g[k>>2]);V=(c[k>>2]=S,+g[k>>2]);W=(c[k>>2]=T,+g[k>>2]);if(y<1.1920928955078125e-7){X=B;Y=D}else{Z=1.0/y;X=B*Z;Y=D*Z}T=b+196|0;Z=-0.0-X;S=T|0;g[S>>2]=Y;K=b+200|0;g[K>>2]=Z;D=(w-F)*Y+(E-x)*Z;if(Q){Z=F-C;F=x-U;x=+R(Z*Z+F*F);if(x<1.1920928955078125e-7){_=Z;$=F}else{B=1.0/x;_=Z*B;$=F*B}B=-0.0-_;g[b+188>>2]=$;g[b+192>>2]=B;aa=(w-C)*$+(E-U)*B;ba=Y*_-X*$>=0.0}else{aa=0.0;ba=0}a:do{if(I){$=V-z;_=W-A;B=+R($*$+_*_);if(B<1.1920928955078125e-7){ca=$;da=_}else{U=1.0/B;ca=$*U;da=_*U}U=-0.0-ca;J=b+204|0;g[J>>2]=da;M=b+208|0;g[M>>2]=U;L=X*da-Y*ca>0.0;_=(w-z)*da+(E-A)*U;if(!Q){O=D>=0.0;if(!I){a[b+248|0]=O&1;e=b+212|0;if(O){ea=e;fa=64;break}else{ga=e;fa=65;break}}if(L){do{if(O){a[b+248|0]=1;ha=b+212|0}else{e=_>=0.0;a[b+248|0]=e&1;ia=b+212|0;if(e){ha=ia;break}e=ia;ia=(g[k>>2]=-0.0-Y,c[k>>2]|0);ja=ia|0;U=+X;c[e>>2]=ja;g[e+4>>2]=U;e=b+228|0;c[e>>2]=ja;g[e+4>>2]=U;e=T;ja=b+236|0;ia=c[e+4>>2]|0;c[ja>>2]=c[e>>2];c[ja+4>>2]=ia;break a}}while(0);ia=T;ja=ha;e=c[ia+4>>2]|0;c[ja>>2]=c[ia>>2];c[ja+4>>2]=e;e=b+228|0;U=+(-0.0- +g[S>>2]);$=+(-0.0- +g[K>>2]);g[e>>2]=U;g[e+4>>2]=$;e=b+204|0;ja=b+236|0;ia=c[e+4>>2]|0;c[ja>>2]=c[e>>2];c[ja+4>>2]=ia;break}else{do{if(O){ia=_>=0.0;a[b+248|0]=ia&1;ja=b+212|0;if(!ia){ka=ja;break}ia=T;e=ja;ja=c[ia>>2]|0;la=c[ia+4>>2]|0;c[e>>2]=ja;c[e+4>>2]=la;e=b+228|0;$=+(-0.0-(c[k>>2]=ja,+g[k>>2]));U=+X;g[e>>2]=$;g[e+4>>2]=U;e=b+236|0;c[e>>2]=ja;c[e+4>>2]=la;break a}else{a[b+248|0]=0;ka=b+212|0}}while(0);O=ka;U=+(-0.0-Y);$=+X;g[O>>2]=U;g[O+4>>2]=$;O=b+228|0;$=+(-0.0- +g[b+204>>2]);U=+(-0.0- +g[b+208>>2]);g[O>>2]=$;g[O+4>>2]=U;O=T;la=b+236|0;e=c[O+4>>2]|0;c[la>>2]=c[O>>2];c[la+4>>2]=e;break}}if(ba&L){do{if(aa<0.0&D<0.0){e=_>=0.0;a[b+248|0]=e&1;la=b+212|0;if(e){ma=la;break}e=la;la=(g[k>>2]=-0.0-Y,c[k>>2]|0);O=la|0;U=+X;c[e>>2]=O;g[e+4>>2]=U;e=b+228|0;c[e>>2]=O;g[e+4>>2]=U;e=b+236|0;c[e>>2]=O;g[e+4>>2]=U;break a}else{a[b+248|0]=1;ma=b+212|0}}while(0);e=T;O=ma;la=c[e+4>>2]|0;c[O>>2]=c[e>>2];c[O+4>>2]=la;la=b+188|0;O=b+228|0;e=c[la+4>>2]|0;c[O>>2]=c[la>>2];c[O+4>>2]=e;e=b+204|0;O=b+236|0;la=c[e+4>>2]|0;c[O>>2]=c[e>>2];c[O+4>>2]=la;break}if(ba){do{if(aa<0.0){if(D<0.0){a[b+248|0]=0;na=b+212|0}else{la=_>=0.0;a[b+248|0]=la&1;O=b+212|0;if(la){oa=O;break}else{na=O}}O=na;U=+(-0.0-Y);$=+X;g[O>>2]=U;g[O+4>>2]=$;O=b+228|0;$=+(-0.0- +g[J>>2]);U=+(-0.0- +g[M>>2]);g[O>>2]=$;g[O+4>>2]=U;O=b+236|0;U=+(-0.0- +g[S>>2]);$=+(-0.0- +g[K>>2]);g[O>>2]=U;g[O+4>>2]=$;break a}else{a[b+248|0]=1;oa=b+212|0}}while(0);O=T;la=oa;e=c[O+4>>2]|0;c[la>>2]=c[O>>2];c[la+4>>2]=e;e=b+188|0;la=b+228|0;ja=c[e+4>>2]|0;c[la>>2]=c[e>>2];c[la+4>>2]=ja;ja=b+236|0;la=c[O+4>>2]|0;c[ja>>2]=c[O>>2];c[ja+4>>2]=la;break}if(!L){do{if(aa<0.0|D<0.0){a[b+248|0]=0;pa=b+212|0}else{la=_>=0.0;a[b+248|0]=la&1;ja=b+212|0;if(!la){pa=ja;break}la=T;O=ja;ja=c[la>>2]|0;e=c[la+4>>2]|0;c[O>>2]=ja;c[O+4>>2]=e;O=b+228|0;c[O>>2]=ja;c[O+4>>2]=e;O=b+236|0;c[O>>2]=ja;c[O+4>>2]=e;break a}}while(0);L=pa;$=+(-0.0-Y);U=+X;g[L>>2]=$;g[L+4>>2]=U;L=b+228|0;U=+(-0.0- +g[J>>2]);$=+(-0.0- +g[M>>2]);g[L>>2]=U;g[L+4>>2]=$;L=b+236|0;$=+(-0.0- +g[b+188>>2]);U=+(-0.0- +g[b+192>>2]);g[L>>2]=$;g[L+4>>2]=U;break}do{if(_<0.0){if(aa<0.0){a[b+248|0]=0;qa=b+212|0}else{L=D>=0.0;a[b+248|0]=L&1;e=b+212|0;if(L){ra=e;break}else{qa=e}}e=qa;U=+(-0.0-Y);$=+X;g[e>>2]=U;g[e+4>>2]=$;e=b+228|0;$=+(-0.0- +g[S>>2]);U=+(-0.0- +g[K>>2]);g[e>>2]=$;g[e+4>>2]=U;e=b+236|0;U=+(-0.0- +g[b+188>>2]);$=+(-0.0- +g[b+192>>2]);g[e>>2]=U;g[e+4>>2]=$;break a}else{a[b+248|0]=1;ra=b+212|0}}while(0);M=T;J=ra;e=c[M+4>>2]|0;c[J>>2]=c[M>>2];c[J+4>>2]=e;e=b+228|0;J=c[M+4>>2]|0;c[e>>2]=c[M>>2];c[e+4>>2]=J;J=b+204|0;e=b+236|0;M=c[J+4>>2]|0;c[e>>2]=c[J>>2];c[e+4>>2]=M}else{if(!Q){M=D>=0.0;a[b+248|0]=M&1;e=b+212|0;if(M){ea=e;fa=64;break}else{ga=e;fa=65;break}}e=aa>=0.0;if(ba){do{if(e){a[b+248|0]=1;sa=b+212|0}else{M=D>=0.0;a[b+248|0]=M&1;J=b+212|0;if(M){sa=J;break}M=J;J=(g[k>>2]=-0.0-Y,c[k>>2]|0);L=0;_=+X;c[M>>2]=L|J;g[M+4>>2]=_;M=T;J=b+228|0;O=c[M>>2]|0;ja=c[M+4>>2]|0;c[J>>2]=O;c[J+4>>2]=ja;ja=b+236|0;c[ja>>2]=L|(g[k>>2]=-0.0-(c[k>>2]=O,+g[k>>2]),c[k>>2]|0);g[ja+4>>2]=_;break a}}while(0);ja=T;O=sa;L=c[ja+4>>2]|0;c[O>>2]=c[ja>>2];c[O+4>>2]=L;L=b+188|0;O=b+228|0;ja=c[L+4>>2]|0;c[O>>2]=c[L>>2];c[O+4>>2]=ja;ja=b+236|0;_=+(-0.0- +g[S>>2]);$=+(-0.0- +g[K>>2]);g[ja>>2]=_;g[ja+4>>2]=$;break}else{do{if(e){ja=D>=0.0;a[b+248|0]=ja&1;O=b+212|0;if(!ja){ta=O;break}ja=T;L=O;O=c[ja>>2]|0;J=c[ja+4>>2]|0;c[L>>2]=O;c[L+4>>2]=J;L=b+228|0;c[L>>2]=O;c[L+4>>2]=J;J=b+236|0;$=+(-0.0-(c[k>>2]=O,+g[k>>2]));_=+X;g[J>>2]=$;g[J+4>>2]=_;break a}else{a[b+248|0]=0;ta=b+212|0}}while(0);e=ta;_=+(-0.0-Y);$=+X;g[e>>2]=_;g[e+4>>2]=$;e=T;J=b+228|0;O=c[e+4>>2]|0;c[J>>2]=c[e>>2];c[J+4>>2]=O;O=b+236|0;$=+(-0.0- +g[b+188>>2]);_=+(-0.0- +g[b+192>>2]);g[O>>2]=$;g[O+4>>2]=_;break}}}while(0);if((fa|0)==64){ta=T;sa=ea;ea=c[ta>>2]|0;ba=c[ta+4>>2]|0;c[sa>>2]=ea;c[sa+4>>2]=ba;ba=b+228|0;sa=(g[k>>2]=-0.0-(c[k>>2]=ea,+g[k>>2]),c[k>>2]|0);ea=sa|0;D=+X;c[ba>>2]=ea;g[ba+4>>2]=D;ba=b+236|0;c[ba>>2]=ea;g[ba+4>>2]=D}else if((fa|0)==65){ba=ga;D=+(-0.0-Y);Y=+X;g[ba>>2]=D;g[ba+4>>2]=Y;ba=T;ga=b+228|0;ea=c[ba>>2]|0;sa=c[ba+4>>2]|0;c[ga>>2]=ea;c[ga+4>>2]=sa;ga=b+236|0;c[ga>>2]=ea;c[ga+4>>2]=sa}sa=h+148|0;ga=b+128|0;c[ga>>2]=c[sa>>2];if((c[sa>>2]|0)>0){ea=0;do{Y=+g[f>>2];D=+g[h+20+(ea<<3)>>2];X=+g[j>>2];aa=+g[h+20+(ea<<3)+4>>2];ba=b+(ea<<3)|0;A=+(+g[G>>2]+(Y*D-X*aa));E=+(D*X+Y*aa+ +g[v>>2]);g[ba>>2]=A;g[ba+4>>2]=E;E=+g[f>>2];A=+g[h+84+(ea<<3)>>2];aa=+g[j>>2];Y=+g[h+84+(ea<<3)+4>>2];ba=b+64+(ea<<3)|0;X=+(E*A-aa*Y);D=+(A*aa+E*Y);g[ba>>2]=X;g[ba+4>>2]=D;ea=ea+1|0;}while((ea|0)<(c[sa>>2]|0))}sa=b+244|0;g[sa>>2]=.019999999552965164;ea=d+60|0;c[ea>>2]=0;ba=b+248|0;ta=c[ga>>2]|0;if((ta|0)<=0){i=l;return}D=+g[b+164>>2];X=+g[P>>2];Y=+g[b+212>>2];E=+g[b+216>>2];P=0;aa=3.4028234663852886e+38;do{A=Y*(+g[b+(P<<3)>>2]-D)+E*(+g[b+(P<<3)+4>>2]-X);aa=A<aa?A:aa;P=P+1|0;}while((P|0)<(ta|0));if(aa>.019999999552965164){i=l;return}uf(m,b);ta=c[m>>2]|0;do{if((ta|0)==0){fa=75}else{X=+g[m+8>>2];if(X>+g[sa>>2]){i=l;return}if(!(X>aa*.9800000190734863+.0010000000474974513)){fa=75;break}P=c[m+4>>2]|0;Q=n;ra=d+56|0;if((ta|0)==1){ua=Q;va=ra;fa=77;break}c[ra>>2]=2;ra=c[H+4>>2]|0;c[o>>2]=c[H>>2];c[o+4>>2]=ra;ra=n+8|0;qa=ra;a[ra]=0;ra=P&255;a[qa+1|0]=ra;a[qa+2|0]=0;a[qa+3|0]=1;qa=p+12|0;pa=c[N+4>>2]|0;c[qa>>2]=c[N>>2];c[qa+4>>2]=pa;pa=p+20|0;qa=pa;a[pa]=0;a[qa+1|0]=ra;a[qa+2|0]=0;a[qa+3|0]=1;c[q>>2]=P;qa=P+1|0;ra=(qa|0)<(c[ga>>2]|0)?qa:0;c[q+4>>2]=ra;qa=b+(P<<3)|0;pa=q+8|0;oa=c[qa>>2]|0;na=c[qa+4>>2]|0;c[pa>>2]=oa;c[pa+4>>2]=na;pa=b+(ra<<3)|0;ra=q+16|0;qa=c[pa>>2]|0;ma=c[pa+4>>2]|0;c[ra>>2]=qa;c[ra+4>>2]=ma;ra=b+64+(P<<3)|0;pa=q+24|0;ka=c[ra>>2]|0;ha=c[ra+4>>2]|0;c[pa>>2]=ka;c[pa+4>>2]=ha;wa=ha;xa=ka;ya=oa;za=na;Aa=qa;Ba=ma;Ca=P;Da=0;Ea=Q}}while(0);if((fa|0)==75){ua=n;va=d+56|0;fa=77}do{if((fa|0)==77){c[va>>2]=1;ta=c[ga>>2]|0;if((ta|0)>1){aa=+g[b+216>>2];X=+g[b+212>>2];m=0;E=X*+g[b+64>>2]+aa*+g[b+68>>2];Q=1;while(1){D=X*+g[b+64+(Q<<3)>>2]+aa*+g[b+64+(Q<<3)+4>>2];P=D<E;ma=P?Q:m;qa=Q+1|0;if((qa|0)<(ta|0)){m=ma;E=P?D:E;Q=qa}else{Fa=ma;break}}}else{Fa=0}Q=Fa+1|0;m=(Q|0)<(ta|0)?Q:0;Q=b+(Fa<<3)|0;ma=c[Q+4>>2]|0;c[o>>2]=c[Q>>2];c[o+4>>2]=ma;ma=n+8|0;Q=ma;a[ma]=0;a[Q+1|0]=Fa;a[Q+2|0]=1;a[Q+3|0]=0;Q=b+(m<<3)|0;ma=p+12|0;qa=c[Q+4>>2]|0;c[ma>>2]=c[Q>>2];c[ma+4>>2]=qa;qa=p+20|0;ma=qa;a[qa]=0;a[ma+1|0]=m;a[ma+2|0]=1;a[ma+3|0]=0;ma=q|0;if((a[ba]|0)==0){c[ma>>2]=1;c[q+4>>2]=0;m=q+8|0;qa=c[N>>2]|0;Q=c[N+4>>2]|0;c[m>>2]=qa;c[m+4>>2]=Q;m=q+16|0;P=c[H>>2]|0;na=c[H+4>>2]|0;c[m>>2]=P;c[m+4>>2]=na;m=q+24|0;oa=(g[k>>2]=-0.0- +g[S>>2],c[k>>2]|0);ka=(g[k>>2]=-0.0- +g[K>>2],c[k>>2]|0);c[m>>2]=oa;c[m+4>>2]=ka;wa=ka;xa=oa;ya=qa;za=Q;Aa=P;Ba=na;Ca=1;Da=1;Ea=ua;break}else{c[ma>>2]=0;c[q+4>>2]=1;ma=q+8|0;na=c[H>>2]|0;P=c[H+4>>2]|0;c[ma>>2]=na;c[ma+4>>2]=P;ma=q+16|0;Q=c[N>>2]|0;qa=c[N+4>>2]|0;c[ma>>2]=Q;c[ma+4>>2]=qa;ma=T;oa=q+24|0;ka=c[ma>>2]|0;m=c[ma+4>>2]|0;c[oa>>2]=ka;c[oa+4>>2]=m;wa=m;xa=ka;ya=na;za=P;Aa=Q;Ba=qa;Ca=0;Da=1;Ea=ua;break}}}while(0);E=(c[k>>2]=wa,+g[k>>2]);aa=(c[k>>2]=xa,+g[k>>2]);X=(c[k>>2]=ya,+g[k>>2]);D=(c[k>>2]=za,+g[k>>2]);Y=(c[k>>2]=Aa,+g[k>>2]);A=(c[k>>2]=Ba,+g[k>>2]);Ba=q+32|0;Aa=q+24|0;za=q+28|0;ya=Aa|0;da=-0.0-aa;g[Ba>>2]=E;g[q+36>>2]=da;wa=q+44|0;aa=-0.0-E;ua=wa;g[ua>>2]=aa;c[ua+4>>2]=xa;ua=q+8|0;T=ua|0;N=q+12|0;z=E*X+D*da;g[q+40>>2]=z;H=q+52|0;g[H>>2]=Y*aa+(c[k>>2]=xa,+g[k>>2])*A;xa=r|0;r=q|0;if((zf(xa,Ea,Ba,z,Ca)|0)<2){i=l;return}if((zf(s,xa,wa,+g[H>>2],c[q+4>>2]|0)|0)<2){i=l;return}q=d+40|0;do{if(Da){H=Aa;wa=q;xa=c[H>>2]|0;Ca=c[H+4>>2]|0;c[wa>>2]=xa;c[wa+4>>2]=Ca;Ca=ua;wa=d+48|0;H=c[Ca>>2]|0;Ba=c[Ca+4>>2]|0;c[wa>>2]=H;c[wa+4>>2]=Ba;z=(c[k>>2]=H,+g[k>>2]);A=(c[k>>2]=xa,+g[k>>2]);aa=+g[N>>2];Y=+g[za>>2];da=+g[s>>2];D=+g[u+4>>2];X=+g[sa>>2];if((da-z)*A+(D-aa)*Y>X){Ga=0;Ha=X}else{X=da- +g[G>>2];da=D- +g[v>>2];D=+g[f>>2];E=+g[j>>2];xa=d;w=+(X*D+da*E);ca=+(D*da+X*(-0.0-E));g[xa>>2]=w;g[xa+4>>2]=ca;c[d+16>>2]=c[s+8>>2];Ga=1;Ha=+g[sa>>2]}ca=+g[u+12>>2];w=+g[s+16>>2];if((ca-z)*A+(w-aa)*Y>Ha){Ia=Ga;break}Y=ca- +g[G>>2];ca=w- +g[v>>2];w=+g[f>>2];aa=+g[j>>2];xa=d+(Ga*20|0)|0;A=+(Y*w+ca*aa);z=+(w*ca+Y*(-0.0-aa));g[xa>>2]=A;g[xa+4>>2]=z;c[d+(Ga*20|0)+16>>2]=c[u+20>>2];Ia=Ga+1|0}else{xa=c[r>>2]|0;H=h+84+(xa<<3)|0;Ba=q;wa=c[H+4>>2]|0;c[Ba>>2]=c[H>>2];c[Ba+4>>2]=wa;wa=h+20+(xa<<3)|0;xa=d+48|0;Ba=c[wa+4>>2]|0;c[xa>>2]=c[wa>>2];c[xa+4>>2]=Ba;z=+g[T>>2];A=+g[ya>>2];aa=+g[N>>2];Y=+g[za>>2];ca=+g[sa>>2];if((+g[s>>2]-z)*A+(+g[u+4>>2]-aa)*Y>ca){Ja=0;Ka=ca}else{Ba=d;xa=c[t+4>>2]|0;c[Ba>>2]=c[t>>2];c[Ba+4>>2]=xa;xa=s+8|0;Ba=xa;wa=d+16|0;H=wa;a[H+2|0]=a[Ba+3|0]|0;a[H+3|0]=a[Ba+2|0]|0;a[wa]=a[Ba+1|0]|0;a[H+1|0]=a[xa]|0;Ja=1;Ka=+g[sa>>2]}xa=u+12|0;if((+g[xa>>2]-z)*A+(+g[s+16>>2]-aa)*Y>Ka){Ia=Ja;break}H=xa;xa=d+(Ja*20|0)|0;Ba=c[H+4>>2]|0;c[xa>>2]=c[H>>2];c[xa+4>>2]=Ba;Ba=u+20|0;xa=Ba;H=d+(Ja*20|0)+16|0;wa=H;a[wa+2|0]=a[xa+3|0]|0;a[wa+3|0]=a[xa+2|0]|0;a[H]=a[xa+1|0]|0;a[wa+1|0]=a[Ba]|0;Ia=Ja+1|0}}while(0);c[ea>>2]=Ia;i=l;return}function uf(a,b){a=a|0;b=b|0;var d=0,e=0,f=0,h=0.0,i=0.0,j=0.0,k=0.0,l=0.0,m=0.0,n=0.0,o=0,p=0,q=0,r=0,s=0,t=0.0,u=0.0,v=0.0,w=0.0,x=0.0,y=0.0,z=0.0,A=0.0,B=0.0,C=0,D=0.0,E=0;d=a|0;c[d>>2]=0;e=a+4|0;c[e>>2]=-1;f=a+8|0;g[f>>2]=-3.4028234663852886e+38;h=+g[b+216>>2];i=+g[b+212>>2];a=c[b+128>>2]|0;if((a|0)<=0){return}j=+g[b+164>>2];k=+g[b+168>>2];l=+g[b+172>>2];m=+g[b+176>>2];n=+g[b+244>>2];o=b+228|0;p=b+232|0;q=b+236|0;r=b+240|0;s=0;t=-3.4028234663852886e+38;while(1){u=+g[b+64+(s<<3)>>2];v=-0.0-u;w=-0.0- +g[b+64+(s<<3)+4>>2];x=+g[b+(s<<3)>>2];y=+g[b+(s<<3)+4>>2];z=(x-j)*v+(y-k)*w;A=(x-l)*v+(y-m)*w;B=z<A?z:A;if(B>n){break}if(h*u+i*w<0.0){if((v- +g[o>>2])*i+(w- +g[p>>2])*h>=-.03490658849477768&B>t){C=8}else{D=t}}else{if((v- +g[q>>2])*i+(w- +g[r>>2])*h>=-.03490658849477768&B>t){C=8}else{D=t}}if((C|0)==8){C=0;c[d>>2]=2;c[e>>2]=s;g[f>>2]=B;D=B}E=s+1|0;if((E|0)<(a|0)){s=E;t=D}else{C=10;break}}if((C|0)==10){return}c[d>>2]=2;c[e>>2]=s;g[f>>2]=B;return}function vf(a,b,c,d,e){a=a|0;b=b|0;c=c|0;d=d|0;e=e|0;var f=0;f=i;i=i+256|0;tf(f|0,a,b,c,d,e);i=f;return}function wf(b,d,e,f,h){b=b|0;d=d|0;e=e|0;f=f|0;h=h|0;var j=0,k=0,l=0,m=0,n=0,o=0,p=0,q=0,r=0,s=0,t=0.0,u=0.0,v=0.0,w=0.0,x=0.0,y=0.0,z=0.0,A=0.0,B=0.0,C=0,D=0,E=0,F=0,G=0,H=0.0,I=0.0,J=0.0,K=0.0,L=0.0,M=0.0,N=0.0,O=0.0,P=0,Q=0.0,S=0.0,T=0.0,U=0.0,V=0.0,W=0,X=0,Y=0;j=i;i=i+40|0;k=j|0;l=j+8|0;m=j+16|0;n=m|0;o=i;i=i+8|0;p=i;i=i+24|0;q=i;i=i+24|0;r=i;i=i+8|0;s=b+60|0;c[s>>2]=0;t=+g[d+8>>2]+ +g[f+8>>2];c[k>>2]=0;u=+xf(k,d,e,f,h);if(u>t){i=j;return}c[l>>2]=0;v=+xf(l,f,h,d,e);if(v>t){i=j;return}if(v>u+.0005000000237487257){u=+g[h>>2];v=+g[h+4>>2];w=+g[h+8>>2];x=+g[h+12>>2];y=+g[e>>2];z=+g[e+4>>2];A=+g[e+8>>2];B=+g[e+12>>2];C=c[l>>2]|0;c[b+56>>2]=2;D=f;E=d;F=C;G=1;H=y;I=z;J=A;K=B;L=u;M=v;N=w;O=x}else{x=+g[e>>2];w=+g[e+4>>2];v=+g[e+8>>2];u=+g[e+12>>2];B=+g[h>>2];A=+g[h+4>>2];z=+g[h+8>>2];y=+g[h+12>>2];h=c[k>>2]|0;c[b+56>>2]=1;D=d;E=f;F=h;G=0;H=B;I=A;J=z;K=y;L=x;M=w;N=v;O=u}h=m;f=c[E+148>>2]|0;u=+g[D+84+(F<<3)>>2];v=+g[D+84+(F<<3)+4>>2];w=O*u-N*v;x=N*u+O*v;v=K*w+J*x;u=-0.0-J;y=K*x+w*u;if((f|0)>0){d=0;w=3.4028234663852886e+38;k=0;while(1){x=v*+g[E+84+(d<<3)>>2]+y*+g[E+84+(d<<3)+4>>2];e=x<w;C=e?d:k;l=d+1|0;if((l|0)<(f|0)){d=l;w=e?x:w;k=C}else{P=C;break}}}else{P=0}k=P+1|0;d=(k|0)<(f|0)?k:0;w=+g[E+20+(P<<3)>>2];y=+g[E+20+(P<<3)+4>>2];v=+(H+(K*w-J*y));x=+(I+(J*w+K*y));g[n>>2]=v;g[n+4>>2]=x;n=F&255;k=m+8|0;m=k;a[k]=n;a[m+1|0]=P;a[m+2|0]=1;a[m+3|0]=0;x=+g[E+20+(d<<3)>>2];v=+g[E+20+(d<<3)+4>>2];E=h+12|0;y=+(H+(K*x-J*v));w=+(I+(J*x+K*v));g[E>>2]=y;g[E+4>>2]=w;E=h+20|0;m=E;a[E]=n;a[m+1|0]=d;a[m+2|0]=1;a[m+3|0]=0;m=F+1|0;d=(m|0)<(c[D+148>>2]|0)?m:0;m=D+20+(F<<3)|0;w=+g[m>>2];y=+g[m+4>>2];m=D+20+(d<<3)|0;v=+g[m>>2];x=+g[m+4>>2];z=v-w;A=x-y;B=+R(z*z+A*A);if(B<1.1920928955078125e-7){Q=z;S=A}else{T=1.0/B;Q=z*T;S=A*T}T=O*Q-N*S;A=O*S+N*Q;g[o>>2]=T;g[o+4>>2]=A;z=T*-1.0;B=L+(O*w-N*y);U=M+(N*w+O*y);m=p|0;V=B*A+U*z;g[r>>2]=-0.0-T;g[r+4>>2]=-0.0-A;if((zf(m,h,r,t-(B*T+U*A),F)|0)<2){i=j;return}if((zf(q|0,m,o,t+((L+(O*v-N*x))*T+(M+(N*v+O*x))*A),d)|0)<2){i=j;return}d=b+40|0;O=+S;S=+(Q*-1.0);g[d>>2]=O;g[d+4>>2]=S;d=b+48|0;S=+((w+v)*.5);v=+((y+x)*.5);g[d>>2]=S;g[d+4>>2]=v;v=+g[q>>2];S=+g[q+4>>2];d=A*v+z*S-V>t;do{if(G<<24>>24==0){if(d){W=0}else{x=v-H;y=S-I;o=b;w=+(K*x+J*y);O=+(x*u+K*y);g[o>>2]=w;g[o+4>>2]=O;c[b+16>>2]=c[q+8>>2];W=1}O=+g[q+12>>2];w=+g[q+16>>2];if(A*O+z*w-V>t){X=W;break}y=O-H;O=w-I;o=b+(W*20|0)|0;w=+(K*y+J*O);x=+(y*u+K*O);g[o>>2]=w;g[o+4>>2]=x;c[b+(W*20|0)+16>>2]=c[q+20>>2];X=W+1|0}else{if(d){Y=0}else{x=v-H;w=S-I;o=b;O=+(K*x+J*w);y=+(x*u+K*w);g[o>>2]=O;g[o+4>>2]=y;o=b+16|0;m=c[q+8>>2]|0;c[o>>2]=m;F=o;a[o]=m>>>8;a[F+1|0]=m;a[F+2|0]=m>>>24;a[F+3|0]=m>>>16;Y=1}y=+g[q+12>>2];O=+g[q+16>>2];if(A*y+z*O-V>t){X=Y;break}w=y-H;y=O-I;m=b+(Y*20|0)|0;O=+(K*w+J*y);x=+(w*u+K*y);g[m>>2]=O;g[m+4>>2]=x;m=b+(Y*20|0)+16|0;F=c[q+20>>2]|0;c[m>>2]=F;o=m;a[m]=F>>>8;a[o+1|0]=F;a[o+2|0]=F>>>24;a[o+3|0]=F>>>16;X=Y+1|0}}while(0);c[s>>2]=X;i=j;return}function xf(a,b,d,e,f){a=a|0;b=b|0;d=d|0;e=e|0;f=f|0;var h=0,i=0,j=0.0,k=0.0,l=0.0,m=0.0,n=0.0,o=0.0,p=0.0,q=0.0,r=0.0,s=0,t=0.0,u=0,v=0,w=0,x=0,y=0.0,z=0.0;h=c[b+148>>2]|0;i=c[e+148>>2]|0;j=+g[f+12>>2];k=+g[d+8>>2];l=+g[f+8>>2];m=+g[d+12>>2];n=j*k-l*m;o=k*l+j*m;m=+g[d>>2]- +g[f>>2];k=+g[d+4>>2]- +g[f+4>>2];p=j*m+l*k;q=m*(-0.0-l)+j*k;if((h|0)<=0){r=-3.4028234663852886e+38;s=0;c[a>>2]=s;return+r}if((i|0)>0){t=-3.4028234663852886e+38;u=0;v=0}else{k=-3.4028234663852886e+38;f=0;d=0;while(1){w=k<3.4028234663852886e+38;j=w?3.4028234663852886e+38:k;x=w?f:d;w=f+1|0;if((w|0)<(h|0)){k=j;f=w;d=x}else{r=j;s=x;break}}c[a>>2]=s;return+r}while(1){k=+g[b+84+(u<<3)>>2];j=+g[b+84+(u<<3)+4>>2];l=o*k-n*j;m=n*k+o*j;j=+g[b+20+(u<<3)>>2];k=+g[b+20+(u<<3)+4>>2];y=p+(o*j-n*k);z=q+(n*j+o*k);k=3.4028234663852886e+38;d=0;do{j=l*(+g[e+20+(d<<3)>>2]-y)+m*(+g[e+20+(d<<3)+4>>2]-z);k=j<k?j:k;d=d+1|0;}while((d|0)<(i|0));d=k>t;z=d?k:t;f=d?u:v;d=u+1|0;if((d|0)<(h|0)){t=z;u=d;v=f}else{r=z;s=f;break}}c[a>>2]=s;return+r}function yf(a,b,d,e,f,h){a=a|0;b=b|0;d=d|0;e=+e;f=f|0;h=+h;var i=0,j=0,k=0,l=0.0,m=0.0,n=0,o=0.0,p=0.0,q=0.0,r=0.0,s=0,t=0.0,u=0.0,v=0,w=0,x=0,y=0,z=0,A=0.0,B=0.0,C=0.0,D=0.0,E=0,F=0.0,G=0.0,H=0.0,I=0.0;i=b+60|0;if((c[i>>2]|0)==0){return}j=c[b+56>>2]|0;if((j|0)==2){k=f+12|0;l=+g[k>>2];m=+g[b+40>>2];n=f+8|0;o=+g[n>>2];p=+g[b+44>>2];q=l*m-o*p;r=m*o+l*p;s=a;p=+q;l=+r;g[s>>2]=p;g[s+4>>2]=l;l=+g[k>>2];p=+g[b+48>>2];o=+g[n>>2];m=+g[b+52>>2];t=+g[f>>2]+(l*p-o*m);u=p*o+l*m+ +g[f+4>>2];if((c[i>>2]|0)>0){n=d+12|0;k=d+8|0;v=d|0;w=d+4|0;x=a|0;y=a+4|0;z=0;m=q;l=r;while(1){o=+g[n>>2];p=+g[b+(z*20|0)>>2];A=+g[k>>2];B=+g[b+(z*20|0)+4>>2];C=+g[v>>2]+(o*p-A*B);D=p*A+o*B+ +g[w>>2];B=h-(m*(C-t)+(D-u)*l);o=C+m*B;A=D+l*B;B=C-m*e;C=D-l*e;E=a+8+(z<<3)|0;D=+((B+o)*.5);p=+((C+A)*.5);g[E>>2]=D;g[E+4>>2]=p;g[a+24+(z<<2)>>2]=+g[x>>2]*(B-o)+ +g[y>>2]*(C-A);E=z+1|0;A=+g[x>>2];C=+g[y>>2];if((E|0)<(c[i>>2]|0)){z=E;m=A;l=C}else{F=A;G=C;break}}}else{F=q;G=r}r=+(-0.0-F);F=+(-0.0-G);g[s>>2]=r;g[s+4>>2]=F;return}else if((j|0)==0){s=a|0;g[s>>2]=1.0;z=a+4|0;g[z>>2]=0.0;F=+g[d+12>>2];r=+g[b+48>>2];G=+g[d+8>>2];q=+g[b+52>>2];l=+g[d>>2]+(F*r-G*q);m=r*G+F*q+ +g[d+4>>2];q=+g[f+12>>2];F=+g[b>>2];G=+g[f+8>>2];r=+g[b+4>>2];u=+g[f>>2]+(q*F-G*r);t=F*G+q*r+ +g[f+4>>2];r=l-u;q=m-t;do{if(r*r+q*q>1.4210854715202004e-14){G=u-l;F=t-m;y=a;C=+G;A=+F;g[y>>2]=C;g[y+4>>2]=A;A=+R(G*G+F*F);if(A<1.1920928955078125e-7){H=G;I=F;break}C=1.0/A;A=G*C;g[s>>2]=A;G=F*C;g[z>>2]=G;H=A;I=G}else{H=1.0;I=0.0}}while(0);q=l+H*e;l=m+I*e;m=u-H*h;u=t-I*h;z=a+8|0;t=+((q+m)*.5);r=+((l+u)*.5);g[z>>2]=t;g[z+4>>2]=r;g[a+24>>2]=H*(m-q)+I*(u-l);return}else if((j|0)==1){j=d+12|0;l=+g[j>>2];u=+g[b+40>>2];z=d+8|0;I=+g[z>>2];q=+g[b+44>>2];m=l*u-I*q;H=u*I+l*q;s=a;q=+m;l=+H;g[s>>2]=q;g[s+4>>2]=l;l=+g[j>>2];q=+g[b+48>>2];I=+g[z>>2];u=+g[b+52>>2];r=+g[d>>2]+(l*q-I*u);t=q*I+l*u+ +g[d+4>>2];if((c[i>>2]|0)<=0){return}d=f+12|0;z=f+8|0;j=f|0;s=f+4|0;f=a|0;y=a+4|0;x=0;u=m;m=H;while(1){H=+g[d>>2];l=+g[b+(x*20|0)>>2];I=+g[z>>2];q=+g[b+(x*20|0)+4>>2];G=+g[j>>2]+(H*l-I*q);A=l*I+H*q+ +g[s>>2];q=e-(u*(G-r)+(A-t)*m);H=G+u*q;I=A+m*q;q=G-u*h;G=A-m*h;w=a+8+(x<<3)|0;A=+((q+H)*.5);l=+((G+I)*.5);g[w>>2]=A;g[w+4>>2]=l;g[a+24+(x<<2)>>2]=+g[f>>2]*(q-H)+ +g[y>>2]*(G-I);w=x+1|0;if((w|0)>=(c[i>>2]|0)){break}x=w;u=+g[f>>2];m=+g[y>>2]}return}else{return}}function zf(b,d,e,f,h){b=b|0;d=d|0;e=e|0;f=+f;h=h|0;var i=0.0,j=0,k=0.0,l=0.0,m=0,n=0,o=0,p=0.0,q=0,r=0,s=0,t=0,u=0;i=+g[e>>2];j=d|0;k=+g[e+4>>2];e=d+4|0;l=i*+g[j>>2]+k*+g[e>>2]-f;m=d+12|0;n=m|0;o=d+16|0;p=i*+g[n>>2]+k*+g[o>>2]-f;if(l>0.0){q=0}else{r=b;s=d;c[r>>2]=c[s>>2];c[r+4>>2]=c[s+4>>2];c[r+8>>2]=c[s+8>>2];q=1}if(p>0.0){t=q}else{s=b+(q*12|0)|0;r=m;c[s>>2]=c[r>>2];c[s+4>>2]=c[r+4>>2];c[s+8>>2]=c[r+8>>2];t=q+1|0}if(!(l*p<0.0)){u=t;return u|0}f=l/(l-p);p=+g[j>>2];l=+g[e>>2];e=b+(t*12|0)|0;k=+(p+f*(+g[n>>2]-p));p=+(l+f*(+g[o>>2]-l));g[e>>2]=k;g[e+4>>2]=p;e=b+(t*12|0)+8|0;b=e;a[e]=h;a[b+1|0]=a[d+9|0]|0;a[b+2|0]=0;a[b+3|0]=1;u=t+1|0;return u|0}function Af(d,e,f,h,j,k){d=d|0;e=e|0;f=f|0;h=h|0;j=j|0;k=k|0;var l=0,m=0,n=0,o=0;l=i;i=i+136|0;m=l|0;n=l+96|0;o=l+112|0;c[m+16>>2]=0;c[m+20>>2]=0;g[m+24>>2]=0.0;c[m+44>>2]=0;c[m+48>>2]=0;g[m+52>>2]=0.0;qe(m|0,d,e);qe(m+28|0,f,h);h=m+56|0;f=j;c[h>>2]=c[f>>2];c[h+4>>2]=c[f+4>>2];c[h+8>>2]=c[f+8>>2];c[h+12>>2]=c[f+12>>2];f=m+72|0;h=k;c[f>>2]=c[h>>2];c[f+4>>2]=c[h+4>>2];c[f+8>>2]=c[h+8>>2];c[f+12>>2]=c[h+12>>2];a[m+88|0]=1;b[n+4>>1]=0;se(o,n,m);i=l;return+g[o+16>>2]<11920928955078125.0e-22|0}function Bf(a,b,d,e,f,g){a=a|0;b=b|0;d=d|0;e=e|0;f=f|0;g=g|0;var h=0,i=0;h=a+40|0;c[h>>2]=b;c[a+44>>2]=d;c[a+48>>2]=e;c[a+28>>2]=0;c[a+36>>2]=0;c[a+32>>2]=0;i=a|0;c[i>>2]=f;c[a+4>>2]=g;c[a+8>>2]=on(f,b<<2)|0;c[a+12>>2]=on(c[i>>2]|0,d<<2)|0;c[a+16>>2]=on(c[i>>2]|0,e<<2)|0;c[a+24>>2]=on(c[i>>2]|0,(c[h>>2]|0)*12|0)|0;c[a+20>>2]=on(c[i>>2]|0,(c[h>>2]|0)*12|0)|0;return}function Cf(a){a=a|0;var b=0;b=a|0;qn(c[b>>2]|0,c[a+20>>2]|0);qn(c[b>>2]|0,c[a+24>>2]|0);qn(c[b>>2]|0,c[a+16>>2]|0);qn(c[b>>2]|0,c[a+12>>2]|0);qn(c[b>>2]|0,c[a+8>>2]|0);return}function Df(d,e,f,h,j){d=d|0;e=e|0;f=f|0;h=h|0;j=j|0;var l=0,m=0,n=0,o=0,p=0,q=0,r=0.0,s=0,t=0,u=0,v=0,w=0,x=0,y=0,z=0,A=0,B=0,C=0.0,D=0.0,E=0.0,F=0.0,G=0.0,H=0.0,I=0.0,J=0.0,K=0.0,L=0.0,M=0,N=0,O=0.0,P=0.0,Q=0.0,S=0.0,V=0,W=0,X=0.0,Y=0.0;l=i;i=i+176|0;m=l|0;n=l+24|0;o=l+32|0;p=l+72|0;q=l+120|0;$m(n);r=+g[f>>2];s=d+28|0;if((c[s>>2]|0)>0){t=d+8|0;u=h|0;v=h+4|0;h=d+20|0;w=d+24|0;x=0;while(1){y=c[(c[t>>2]|0)+(x<<2)>>2]|0;z=y+60|0;A=c[z>>2]|0;B=c[z+4>>2]|0;C=+g[y+72>>2];z=y+80|0;D=+g[z>>2];E=+g[z+4>>2];F=+g[y+88>>2];z=y+52|0;c[z>>2]=A;c[z+4>>2]=B;g[y+68>>2]=C;if((c[y>>2]|0)==2){G=+g[y+156>>2];H=+g[y+136>>2];I=1.0/(r*+g[y+148>>2]+1.0);J=(F+r*+g[y+144>>2]*+g[y+100>>2])*(1.0/(r*+g[y+152>>2]+1.0));K=(D+r*(G*+g[u>>2]+H*+g[y+92>>2]))*I;L=(E+r*(G*+g[v>>2]+H*+g[y+96>>2]))*I}else{J=F;K=D;L=E}y=(c[h>>2]|0)+(x*12|0)|0;c[y>>2]=A;c[y+4>>2]=B;g[(c[h>>2]|0)+(x*12|0)+8>>2]=C;B=(c[w>>2]|0)+(x*12|0)|0;C=+K;E=+L;g[B>>2]=C;g[B+4>>2]=E;g[(c[w>>2]|0)+(x*12|0)+8>>2]=J;B=x+1|0;if((B|0)<(c[s>>2]|0)){x=B}else{M=h;N=w;break}}}else{M=d+20|0;N=d+24|0}an(n);w=o;h=f;c[w>>2]=c[h>>2];c[w+4>>2]=c[h+4>>2];c[w+8>>2]=c[h+8>>2];c[w+12>>2]=c[h+12>>2];c[w+16>>2]=c[h+16>>2];c[w+20>>2]=c[h+20>>2];c[w+24>>2]=c[h+24>>2];w=c[M>>2]|0;c[o+28>>2]=w;x=c[N>>2]|0;c[o+32>>2]=x;v=p;c[v>>2]=c[h>>2];c[v+4>>2]=c[h+4>>2];c[v+8>>2]=c[h+8>>2];c[v+12>>2]=c[h+12>>2];c[v+16>>2]=c[h+16>>2];c[v+20>>2]=c[h+20>>2];c[v+24>>2]=c[h+24>>2];h=d+12|0;c[p+28>>2]=c[h>>2];v=d+36|0;c[p+32>>2]=c[v>>2];c[p+36>>2]=w;c[p+40>>2]=x;c[p+44>>2]=c[d>>2];Zi(q,p);$i(q);if((a[f+24|0]|0)!=0){aj(q)}p=d+32|0;if((c[p>>2]|0)>0){x=d+16|0;w=0;do{u=c[(c[x>>2]|0)+(w<<2)>>2]|0;kb[c[(c[u>>2]|0)+32>>2]&255](u,o);w=w+1|0;}while((w|0)<(c[p>>2]|0))}g[e+12>>2]=+bn(n);an(n);w=f+12|0;if((c[w>>2]|0)>0){x=d+16|0;u=0;do{if((c[p>>2]|0)>0){t=0;do{B=c[(c[x>>2]|0)+(t<<2)>>2]|0;kb[c[(c[B>>2]|0)+36>>2]&255](B,o);t=t+1|0;}while((t|0)<(c[p>>2]|0))}bj(q);u=u+1|0;}while((u|0)<(c[w>>2]|0))}cj(q);g[e+16>>2]=+bn(n);if((c[s>>2]|0)>0){w=0;u=c[N>>2]|0;do{x=c[M>>2]|0;t=x+(w*12|0)|0;J=+g[t>>2];L=+g[t+4>>2];K=+g[x+(w*12|0)+8>>2];x=u+(w*12|0)|0;E=+g[x>>2];C=+g[x+4>>2];D=+g[u+(w*12|0)+8>>2];F=r*E;I=r*C;H=F*F+I*I;if(H>4.0){I=2.0/+R(H);O=E*I;P=C*I}else{O=E;P=C}C=r*D;if(C*C>2.4674012660980225){if(C>0.0){Q=C}else{Q=-0.0-C}S=D*(1.5707963705062866/Q)}else{S=D}D=+(J+r*O);J=+(L+r*P);g[t>>2]=D;g[t+4>>2]=J;g[(c[M>>2]|0)+(w*12|0)+8>>2]=K+r*S;t=(c[N>>2]|0)+(w*12|0)|0;K=+O;J=+P;g[t>>2]=K;g[t+4>>2]=J;u=c[N>>2]|0;g[u+(w*12|0)+8>>2]=S;w=w+1|0;}while((w|0)<(c[s>>2]|0))}an(n);w=f+16|0;f=d+16|0;u=0;while(1){if((u|0)>=(c[w>>2]|0)){V=1;break}t=dj(q)|0;if((c[p>>2]|0)>0){x=1;B=0;while(1){y=c[(c[f>>2]|0)+(B<<2)>>2]|0;A=x&(tb[c[(c[y>>2]|0)+40>>2]&63](y,o)|0);y=B+1|0;if((y|0)<(c[p>>2]|0)){x=A;B=y}else{W=A;break}}}else{W=1}if(t&W){V=0;break}else{u=u+1|0}}if((c[s>>2]|0)>0){u=d+8|0;W=0;do{p=c[(c[u>>2]|0)+(W<<2)>>2]|0;o=(c[M>>2]|0)+(W*12|0)|0;f=p+60|0;w=c[o>>2]|0;B=c[o+4>>2]|0;c[f>>2]=w;c[f+4>>2]=B;S=+g[(c[M>>2]|0)+(W*12|0)+8>>2];g[p+72>>2]=S;f=(c[N>>2]|0)+(W*12|0)|0;o=p+80|0;x=c[f+4>>2]|0;c[o>>2]=c[f>>2];c[o+4>>2]=x;g[p+88>>2]=+g[(c[N>>2]|0)+(W*12|0)+8>>2];P=+U(S);g[p+20>>2]=P;O=+T(S);g[p+24>>2]=O;S=+g[p+44>>2];Q=+g[p+48>>2];J=(c[k>>2]=w,+g[k>>2])-(O*S-P*Q);K=(c[k>>2]=B,+g[k>>2])-(P*S+O*Q);B=p+12|0;Q=+J;J=+K;g[B>>2]=Q;g[B+4>>2]=J;W=W+1|0;}while((W|0)<(c[s>>2]|0))}g[e+20>>2]=+bn(n);n=c[q+44>>2]|0;e=d+4|0;do{if((c[e>>2]|0)!=0){if((c[v>>2]|0)<=0){break}W=m+16|0;N=0;do{M=c[(c[h>>2]|0)+(N<<2)>>2]|0;u=c[n+(N*156|0)+148>>2]|0;c[W>>2]=u;if((u|0)>0){B=0;do{g[m+(B<<2)>>2]=+g[n+(N*156|0)+(B*36|0)+16>>2];g[m+8+(B<<2)>>2]=+g[n+(N*156|0)+(B*36|0)+20>>2];B=B+1|0;}while((B|0)<(u|0))}u=c[e>>2]|0;ob[c[(c[u>>2]|0)+36>>2]&31](u,M,m);N=N+1|0;}while((N|0)<(c[v>>2]|0))}}while(0);if(!j){_i(q);i=l;return}j=c[s>>2]|0;v=(j|0)>0;if(v){m=c[d+8>>2]|0;J=3.4028234663852886e+38;e=0;while(1){n=c[m+(e<<2)>>2]|0;a:do{if((c[n>>2]|0)==0){X=J}else{do{if(!((b[n+4>>1]&4)==0)){Q=+g[n+88>>2];if(Q*Q>.001218469929881394){break}Q=+g[n+80>>2];K=+g[n+84>>2];if(Q*Q+K*K>9999999747378752.0e-20){break}h=n+160|0;K=r+ +g[h>>2];g[h>>2]=K;X=J<K?J:K;break a}}while(0);g[n+160>>2]=0.0;X=0.0}}while(0);n=e+1|0;if((n|0)<(j|0)){J=X;e=n}else{Y=X;break}}}else{Y=3.4028234663852886e+38}if(Y<.5|V|v^1){_i(q);i=l;return}v=d+8|0;d=0;do{V=c[(c[v>>2]|0)+(d<<2)>>2]|0;e=V+4|0;b[e>>1]=b[e>>1]&-3;g[V+160>>2]=0.0;Nn(V+80|0,0,24)|0;d=d+1|0;}while((d|0)<(c[s>>2]|0));_i(q);i=l;return}function Ef(a,b,d,e){a=a|0;b=b|0;d=d|0;e=e|0;var f=0,h=0,j=0,l=0,m=0,n=0,o=0,p=0,q=0,r=0,s=0,t=0,u=0,v=0,w=0,x=0.0,y=0.0,z=0.0,A=0.0,B=0.0,C=0.0,D=0.0,E=0.0,F=0.0,G=0.0,H=0.0,I=0.0,J=0.0,K=0.0;f=i;i=i+128|0;h=f|0;j=f+24|0;l=f+72|0;m=a+28|0;if((c[m>>2]|0)>0){n=a+8|0;o=a+20|0;p=a+24|0;q=0;while(1){r=c[(c[n>>2]|0)+(q<<2)>>2]|0;s=r+60|0;t=(c[o>>2]|0)+(q*12|0)|0;u=c[s+4>>2]|0;c[t>>2]=c[s>>2];c[t+4>>2]=u;g[(c[o>>2]|0)+(q*12|0)+8>>2]=+g[r+72>>2];u=r+80|0;t=(c[p>>2]|0)+(q*12|0)|0;s=c[u+4>>2]|0;c[t>>2]=c[u>>2];c[t+4>>2]=s;s=c[p>>2]|0;g[s+(q*12|0)+8>>2]=+g[r+88>>2];r=q+1|0;if((r|0)<(c[m>>2]|0)){q=r}else{v=s;w=o;break}}}else{v=c[a+24>>2]|0;w=a+20|0}o=a+12|0;c[j+28>>2]=c[o>>2];q=a+36|0;c[j+32>>2]=c[q>>2];c[j+44>>2]=c[a>>2];p=j;n=b;c[p>>2]=c[n>>2];c[p+4>>2]=c[n+4>>2];c[p+8>>2]=c[n+8>>2];c[p+12>>2]=c[n+12>>2];c[p+16>>2]=c[n+16>>2];c[p+20>>2]=c[n+20>>2];c[p+24>>2]=c[n+24>>2];c[j+36>>2]=c[w>>2];n=a+24|0;c[j+40>>2]=v;Zi(l,j);j=b+16|0;v=0;while(1){if((v|0)>=(c[j>>2]|0)){break}if(fj(l,d,e)|0){break}else{v=v+1|0}}v=a+8|0;j=(c[w>>2]|0)+(d*12|0)|0;p=(c[(c[v>>2]|0)+(d<<2)>>2]|0)+52|0;s=c[j+4>>2]|0;c[p>>2]=c[j>>2];c[p+4>>2]=s;s=c[w>>2]|0;p=c[v>>2]|0;g[(c[p+(d<<2)>>2]|0)+68>>2]=+g[s+(d*12|0)+8>>2];d=s+(e*12|0)|0;s=(c[p+(e<<2)>>2]|0)+52|0;p=c[d+4>>2]|0;c[s>>2]=c[d>>2];c[s+4>>2]=p;g[(c[(c[v>>2]|0)+(e<<2)>>2]|0)+68>>2]=+g[(c[w>>2]|0)+(e*12|0)+8>>2];$i(l);e=b+12|0;if((c[e>>2]|0)>0){p=0;do{bj(l);p=p+1|0;}while((p|0)<(c[e>>2]|0))}x=+g[b>>2];if((c[m>>2]|0)>0){b=0;do{e=c[w>>2]|0;p=e+(b*12|0)|0;y=+g[p>>2];z=+g[p+4>>2];A=+g[e+(b*12|0)+8>>2];e=c[n>>2]|0;s=e+(b*12|0)|0;B=+g[s>>2];C=+g[s+4>>2];D=+g[e+(b*12|0)+8>>2];E=x*B;F=x*C;G=E*E+F*F;if(G>4.0){F=2.0/+R(G);H=B*F;I=C*F}else{H=B;I=C}C=x*D;if(C*C>2.4674012660980225){if(C>0.0){J=C}else{J=-0.0-C}K=D*(1.5707963705062866/J)}else{K=D}D=y+x*H;y=z+x*I;z=A+x*K;e=(g[k>>2]=D,c[k>>2]|0);s=e|0;A=+y;c[p>>2]=s;g[p+4>>2]=A;g[(c[w>>2]|0)+(b*12|0)+8>>2]=z;p=(c[n>>2]|0)+(b*12|0)|0;e=(g[k>>2]=H,c[k>>2]|0);d=e|0;C=+I;c[p>>2]=d;g[p+4>>2]=C;g[(c[n>>2]|0)+(b*12|0)+8>>2]=K;p=c[(c[v>>2]|0)+(b<<2)>>2]|0;e=p+60|0;c[e>>2]=s;g[e+4>>2]=A;g[p+72>>2]=z;e=p+80|0;c[e>>2]=d;g[e+4>>2]=C;g[p+88>>2]=K;C=+U(z);g[p+20>>2]=C;A=+T(z);g[p+24>>2]=A;z=+g[p+44>>2];B=+g[p+48>>2];e=p+12|0;F=+(D-(A*z-C*B));D=+(y-(C*z+A*B));g[e>>2]=F;g[e+4>>2]=D;b=b+1|0;}while((b|0)<(c[m>>2]|0))}m=c[l+44>>2]|0;b=a+4|0;if((c[b>>2]|0)==0){_i(l);i=f;return}if((c[q>>2]|0)<=0){_i(l);i=f;return}a=h+16|0;v=0;do{n=c[(c[o>>2]|0)+(v<<2)>>2]|0;w=c[m+(v*156|0)+148>>2]|0;c[a>>2]=w;if((w|0)>0){e=0;do{g[h+(e<<2)>>2]=+g[m+(v*156|0)+(e*36|0)+16>>2];g[h+8+(e<<2)>>2]=+g[m+(v*156|0)+(e*36|0)+20>>2];e=e+1|0;}while((e|0)<(w|0))}w=c[b>>2]|0;ob[c[(c[w>>2]|0)+36>>2]&31](w,n,h);v=v+1|0;}while((v|0)<(c[q>>2]|0));_i(l);i=f;return}function Ff(a,c,d){a=a|0;c=c|0;d=d|0;var e=0;a=b[c+36>>1]|0;if(!(a<<16>>16!=(b[d+36>>1]|0)|a<<16>>16==0)){e=a<<16>>16>0;return e|0}if((b[d+32>>1]&b[c+34>>1])<<16>>16==0){e=0;return e|0}e=(b[d+34>>1]&b[c+32>>1])<<16>>16!=0;return e|0}function Gf(a){a=a|0;return}function Hf(a){a=a|0;Jn(a);return}function If(a,b,c,d){a=a|0;b=b|0;c=c|0;d=d|0;return 1}function Jf(a,b,c,d){a=a|0;b=b|0;c=c|0;d=d|0;return 1}function Kf(a,b){a=a|0;b=b|0;var d=0,e=0,f=0,h=0,i=0.0,j=0.0,l=0.0,m=0.0,n=0.0,o=0.0;Kh(a|0,b|0);c[a>>2]=5600;d=b+20|0;e=a+76|0;f=c[d>>2]|0;h=c[d+4>>2]|0;c[e>>2]=f;c[e+4>>2]=h;e=c[a+52>>2]|0;i=(c[k>>2]=f,+g[k>>2])- +g[e+12>>2];j=(c[k>>2]=h,+g[k>>2])- +g[e+16>>2];l=+g[e+24>>2];m=+g[e+20>>2];e=a+68|0;n=+(i*l+j*m);o=+(l*j+i*(-0.0-m));g[e>>2]=n;g[e+4>>2]=o;g[a+104>>2]=+g[b+28>>2];g[a+96>>2]=0.0;g[a+100>>2]=0.0;g[a+84>>2]=+g[b+32>>2];g[a+88>>2]=+g[b+36>>2];g[a+92>>2]=0.0;g[a+108>>2]=0.0;return}function Lf(a,d){a=a|0;d=d|0;var e=0,f=0,h=0;e=c[a+52>>2]|0;f=e+4|0;h=b[f>>1]|0;if((h&2)==0){b[f>>1]=h|2;g[e+160>>2]=0.0}e=d;d=a+76|0;a=c[e+4>>2]|0;c[d>>2]=c[e>>2];c[d+4>>2]=a;return}function Mf(b,d){b=b|0;d=d|0;var e=0,f=0,h=0,i=0,j=0,l=0,m=0,n=0.0,o=0.0,p=0.0,q=0.0,r=0.0,s=0,t=0.0,u=0.0,v=0.0,w=0.0,x=0.0,y=0.0,z=0.0,A=0.0,B=0.0,C=0.0,D=0.0,E=0.0,F=0.0,G=0.0,H=0,I=0,J=0,K=0,L=0,M=0,N=0,O=0,P=0,Q=0,R=0,S=0,V=0,W=0,X=0,Y=0,Z=0,_=0;e=c[b+52>>2]|0;f=c[e+8>>2]|0;h=b+116|0;c[h>>2]=f;i=e+44|0;j=b+128|0;l=c[i>>2]|0;m=c[i+4>>2]|0;c[j>>2]=l;c[j+4>>2]=m;n=+g[e+136>>2];g[b+136>>2]=n;o=+g[e+144>>2];g[b+140>>2]=o;j=c[d+28>>2]|0;i=j+(f*12|0)|0;p=+g[i>>2];q=+g[i+4>>2];r=+g[j+(f*12|0)+8>>2];j=d+32|0;i=c[j>>2]|0;s=i+(f*12|0)|0;t=+g[s>>2];u=+g[s+4>>2];v=+g[i+(f*12|0)+8>>2];w=+U(r);x=+T(r);r=+g[e+132>>2];y=+g[b+84>>2]*6.2831854820251465;z=+g[d>>2];A=z*r*y*y;B=z*(y*r*2.0*+g[b+88>>2]+A);r=(c[k>>2]=l,+g[k>>2]);y=(c[k>>2]=m,+g[k>>2]);if(B!=0.0){C=1.0/B}else{C=B}g[b+108>>2]=C;B=A*C;g[b+92>>2]=B;A=+g[b+68>>2]-r;r=+g[b+72>>2]-y;y=x*A-w*r;z=w*A+x*r;m=b+120|0;r=+y;x=+z;g[m>>2]=r;g[m+4>>2]=x;x=C+(n+z*o*z);r=z*y*(-0.0-o);A=C+(n+y*o*y);C=x*A-r*r;if(C!=0.0){D=1.0/C}else{D=C}C=r*(-0.0-D);g[b+144>>2]=A*D;g[b+148>>2]=C;g[b+152>>2]=C;g[b+156>>2]=x*D;m=b+160|0;D=p+y- +g[b+76>>2];p=q+z- +g[b+80>>2];l=m;q=+D;x=+p;g[l>>2]=q;g[l+4>>2]=x;g[m>>2]=B*D;g[b+164>>2]=B*p;p=v*.9800000190734863;m=b+96|0;if((a[d+24|0]|0)==0){g[m>>2]=0.0;g[b+100>>2]=0.0;E=p;F=t;G=u;H=c[j>>2]|0;I=H+(f*12|0)|0;J=I;K=(g[k>>2]=F,c[k>>2]|0);L=(g[k>>2]=G,c[k>>2]|0);M=L;N=0;O=0;P=M;Q=K;R=0;S=O|Q;V=P|R;W=J|0;c[W>>2]=S;X=J+4|0;c[X>>2]=V;Y=c[h>>2]|0;Z=c[j>>2]|0;_=Z+(Y*12|0)+8|0;g[_>>2]=E;return}else{v=+g[d+8>>2];d=m|0;B=v*+g[d>>2];g[d>>2]=B;d=b+100|0;D=v*+g[d>>2];g[d>>2]=D;E=p+o*(D*y-B*z);F=t+n*B;G=u+D*n;H=c[j>>2]|0;I=H+(f*12|0)|0;J=I;K=(g[k>>2]=F,c[k>>2]|0);L=(g[k>>2]=G,c[k>>2]|0);M=L;N=0;O=0;P=M;Q=K;R=0;S=O|Q;V=P|R;W=J|0;c[W>>2]=S;X=J+4|0;c[X>>2]=V;Y=c[h>>2]|0;Z=c[j>>2]|0;_=Z+(Y*12|0)+8|0;g[_>>2]=E;return}}function Nf(a,b){a=a|0;b=b|0;var d=0,e=0,f=0,h=0,i=0,j=0.0,k=0.0,l=0.0,m=0.0,n=0.0,o=0.0,p=0.0,q=0,r=0.0,s=0.0,t=0.0,u=0,v=0.0,w=0.0,x=0.0,y=0.0,z=0.0;d=a+116|0;e=c[d>>2]|0;f=b+32|0;h=c[f>>2]|0;i=h+(e*12|0)|0;j=+g[i>>2];k=+g[i+4>>2];l=+g[h+(e*12|0)+8>>2];m=+g[a+124>>2];n=+g[a+120>>2];o=+g[a+108>>2];e=a+96|0;h=e|0;p=+g[h>>2];q=a+100|0;r=+g[q>>2];s=-0.0-(j+m*(-0.0-l)+ +g[a+160>>2]+o*p);t=-0.0-(k+l*n+ +g[a+164>>2]+o*r);o=+g[a+148>>2]*s+ +g[a+156>>2]*t;u=e;v=+g[u>>2];w=+g[u+4>>2];x=p+(+g[a+144>>2]*s+ +g[a+152>>2]*t);g[h>>2]=x;t=o+r;g[q>>2]=t;r=+g[b>>2]*+g[a+104>>2];o=t*t+x*x;if(o>r*r){s=r/+R(o);o=x*s;g[h>>2]=o;r=s*t;g[q>>2]=r;y=o;z=r}else{y=x;z=t}t=y-v;v=z-w;w=+g[a+136>>2];z=l+ +g[a+140>>2]*(v*n-t*m);m=+(j+t*w);t=+(k+v*w);g[i>>2]=m;g[i+4>>2]=t;g[(c[f>>2]|0)+((c[d>>2]|0)*12|0)+8>>2]=z;return}function Of(a,b){a=a|0;b=b|0;return 1}function Pf(a,b){a=a|0;b=b|0;var d=0;d=b+76|0;b=a;a=c[d+4>>2]|0;c[b>>2]=c[d>>2];c[b+4>>2]=a;return}function Qf(a,b){a=a|0;b=b|0;var d=0,e=0.0,f=0.0,h=0.0,i=0.0,j=0.0;d=c[b+52>>2]|0;e=+g[d+24>>2];f=+g[b+68>>2];h=+g[d+20>>2];i=+g[b+72>>2];j=f*h+e*i+ +g[d+16>>2];g[a>>2]=+g[d+12>>2]+(e*f-h*i);g[a+4>>2]=j;return}function Rf(a,b,c){a=a|0;b=b|0;c=+c;var d=0.0;d=+g[b+100>>2]*c;g[a>>2]=+g[b+96>>2]*c;g[a+4>>2]=d;return}function Sf(a,b){a=a|0;b=+b;return+(b*0.0)}function Tf(a,b){a=a|0;b=b|0;var c=0;c=a+76|0;g[c>>2]=+g[c>>2]- +g[b>>2];c=a+80|0;g[c>>2]=+g[c>>2]- +g[b+4>>2];return}function Uf(a){a=a|0;var b=0;a=i;gn(4144,(b=i,i=i+1|0,i=i+7&-8,c[b>>2]=0,b)|0);i=b;i=a;return}function Vf(a){a=a|0;return}function Wf(a){a=a|0;Jn(a);return}function Xf(a,b,d){a=a|0;b=b|0;d=d|0;var e=0,f=0.0,h=0.0,i=0.0,j=0.0,k=0.0,l=0.0;c[a+8>>2]=b;c[a+12>>2]=d;e=d+12|0;f=+g[e>>2];h=f- +g[b+12>>2];f=+g[e+4>>2]- +g[b+16>>2];i=+g[b+24>>2];j=+g[b+20>>2];e=a+20|0;k=+(h*i+f*j);l=+(i*f+h*(-0.0-j));g[e>>2]=k;g[e+4>>2]=l;g[a+28>>2]=+g[d+72>>2]- +g[b+72>>2];return}function Yf(a,b){a=a|0;b=b|0;var d=0,e=0,f=0;Kh(a|0,b|0);c[a>>2]=5664;d=b+20|0;e=a+68|0;f=c[d+4>>2]|0;c[e>>2]=c[d>>2];c[e+4>>2]=f;g[a+76>>2]=+g[b+28>>2];g[a+80>>2]=0.0;g[a+84>>2]=0.0;g[a+88>>2]=0.0;g[a+92>>2]=+g[b+32>>2];g[a+96>>2]=+g[b+36>>2];g[a+100>>2]=+g[b+40>>2];return}function Zf(b,d){b=b|0;d=d|0;var e=0,f=0,h=0,i=0,j=0,l=0,m=0,n=0,o=0,p=0,q=0,r=0,s=0.0,t=0.0,u=0.0,v=0.0,w=0.0,x=0.0,y=0.0,z=0.0,A=0.0,B=0.0,C=0.0,D=0.0,E=0.0,F=0.0,G=0.0,H=0.0,I=0.0,J=0.0,K=0.0,L=0.0,M=0.0,N=0.0,O=0.0,P=0.0,Q=0.0,R=0.0,S=0.0,V=0.0,W=0.0,X=0.0,Y=0.0,Z=0.0,_=0.0,$=0.0,aa=0.0,ba=0,ca=0,da=0,ea=0,fa=0,ga=0,ha=0,ia=0,ja=0,ka=0,la=0,ma=0,na=0,oa=0,pa=0,qa=0,ra=0,sa=0,ta=0,ua=0,va=0,wa=0,xa=0,ya=0,za=0,Aa=0,Ba=0,Ca=0,Da=0,Ea=0,Fa=0,Ga=0,Ha=0,Ia=0,Ja=0,Ka=0;e=c[b+48>>2]|0;f=c[e+8>>2]|0;h=b+104|0;c[h>>2]=f;i=c[b+52>>2]|0;j=c[i+8>>2]|0;l=b+108|0;c[l>>2]=j;m=e+44|0;n=b+128|0;o=c[m>>2]|0;p=c[m+4>>2]|0;c[n>>2]=o;c[n+4>>2]=p;n=i+44|0;m=b+136|0;q=c[n>>2]|0;r=c[n+4>>2]|0;c[m>>2]=q;c[m+4>>2]=r;s=+g[e+136>>2];g[b+156>>2]=s;t=+g[i+136>>2];g[b+160>>2]=t;u=+g[e+144>>2];g[b+164>>2]=u;v=+g[i+144>>2];g[b+168>>2]=v;i=c[d+28>>2]|0;e=i+(f*12|0)|0;w=+g[e>>2];x=+g[e+4>>2];y=+g[i+(f*12|0)+8>>2];e=d+32|0;m=c[e>>2]|0;n=m+(f*12|0)|0;z=+g[n>>2];A=+g[n+4>>2];B=+g[m+(f*12|0)+8>>2];n=i+(j*12|0)|0;C=+g[n>>2];D=+g[n+4>>2];E=+g[i+(j*12|0)+8>>2];i=m+(j*12|0)|0;F=+g[i>>2];G=+g[i+4>>2];H=+g[m+(j*12|0)+8>>2];I=+U(y);J=+T(y);K=+U(E);L=+T(E);M=-0.0-(c[k>>2]=o,+g[k>>2]);N=-0.0-(c[k>>2]=p,+g[k>>2]);O=J*M-I*N;P=I*M+J*N;p=b+112|0;N=+O;M=+P;g[p>>2]=N;g[p+4>>2]=M;M=-0.0-(c[k>>2]=q,+g[k>>2]);N=-0.0-(c[k>>2]=r,+g[k>>2]);Q=L*M-K*N;R=K*M+L*N;r=b+120|0;N=+Q;L=+R;g[r>>2]=N;g[r+4>>2]=L;L=s+t;N=L+P*u*P+R*v*R;M=v*Q;K=P*O*(-0.0-u)-R*M;S=L+O*u*O+Q*M;M=N*S-K*K;if(M!=0.0){V=1.0/M}else{V=M}M=K*(-0.0-V);g[b+172>>2]=S*V;g[b+176>>2]=M;g[b+180>>2]=M;g[b+184>>2]=N*V;V=u+v;if(V>0.0){W=1.0/V}else{W=V}g[b+188>>2]=W;W=+g[b+68>>2];V=+g[b+72>>2];r=b+144|0;N=+(C+Q-w-O-(J*W-I*V));w=+(D+R-x-P-(I*W+J*V));g[r>>2]=N;g[r+4>>2]=w;g[b+152>>2]=E-y- +g[b+76>>2];r=b+80|0;if((a[d+24|0]|0)==0){g[r>>2]=0.0;g[b+84>>2]=0.0;g[b+88>>2]=0.0;X=B;Y=H;Z=F;_=G;$=z;aa=A;ba=c[e>>2]|0;ca=ba+(f*12|0)|0;da=ca;ea=(g[k>>2]=$,c[k>>2]|0);fa=(g[k>>2]=aa,c[k>>2]|0);ga=fa;ha=0;ia=0;ja=ga;ka=ea;la=0;ma=ia|ka;na=ja|la;oa=da|0;c[oa>>2]=ma;pa=da+4|0;c[pa>>2]=na;qa=c[h>>2]|0;ra=c[e>>2]|0;sa=ra+(qa*12|0)+8|0;g[sa>>2]=X;ta=c[l>>2]|0;ua=ra+(ta*12|0)|0;va=ua;wa=(g[k>>2]=Z,c[k>>2]|0);xa=(g[k>>2]=_,c[k>>2]|0);ya=xa;za=0;Aa=0;Ba=ya;Ca=wa;Da=0;Ea=Aa|Ca;Fa=Ba|Da;Ga=va|0;c[Ga>>2]=Ea;Ha=va+4|0;c[Ha>>2]=Fa;Ia=c[l>>2]|0;Ja=c[e>>2]|0;Ka=Ja+(Ia*12|0)+8|0;g[Ka>>2]=Y;return}else{q=d+8|0;y=+g[q>>2];d=r|0;E=y*+g[d>>2];g[d>>2]=E;d=b+84|0;w=y*+g[d>>2];g[d>>2]=w;d=b+88|0;y=+g[q>>2]*+g[d>>2];g[d>>2]=y;X=B-u*(y+(w*O-E*P));Y=H+v*(y+(w*Q-E*R));Z=F+t*E;_=G+t*w;$=z-s*E;aa=A-s*w;ba=c[e>>2]|0;ca=ba+(f*12|0)|0;da=ca;ea=(g[k>>2]=$,c[k>>2]|0);fa=(g[k>>2]=aa,c[k>>2]|0);ga=fa;ha=0;ia=0;ja=ga;ka=ea;la=0;ma=ia|ka;na=ja|la;oa=da|0;c[oa>>2]=ma;pa=da+4|0;c[pa>>2]=na;qa=c[h>>2]|0;ra=c[e>>2]|0;sa=ra+(qa*12|0)+8|0;g[sa>>2]=X;ta=c[l>>2]|0;ua=ra+(ta*12|0)|0;va=ua;wa=(g[k>>2]=Z,c[k>>2]|0);xa=(g[k>>2]=_,c[k>>2]|0);ya=xa;za=0;Aa=0;Ba=ya;Ca=wa;Da=0;Ea=Aa|Ca;Fa=Ba|Da;Ga=va|0;c[Ga>>2]=Ea;Ha=va+4|0;c[Ha>>2]=Fa;Ia=c[l>>2]|0;Ja=c[e>>2]|0;Ka=Ja+(Ia*12|0)+8|0;g[Ka>>2]=Y;return}}function _f(a,b){a=a|0;b=b|0;var d=0,e=0,f=0,h=0,i=0,j=0.0,k=0.0,l=0.0,m=0,n=0,o=0.0,p=0.0,q=0.0,r=0.0,s=0.0,t=0.0,u=0.0,v=0.0,w=0.0,x=0.0,y=0.0,z=0.0,A=0.0,B=0.0,C=0.0,D=0.0,E=0.0,F=0.0,G=0.0,H=0.0,I=0.0,J=0.0,K=0.0;d=a+104|0;e=c[d>>2]|0;f=b+32|0;h=c[f>>2]|0;i=h+(e*12|0)|0;j=+g[i>>2];k=+g[i+4>>2];l=+g[h+(e*12|0)+8>>2];e=a+108|0;m=c[e>>2]|0;n=h+(m*12|0)|0;o=+g[n>>2];p=+g[n+4>>2];q=+g[h+(m*12|0)+8>>2];r=+g[a+156>>2];s=+g[a+160>>2];t=+g[a+164>>2];u=+g[a+168>>2];v=+g[b>>2];w=+g[b+4>>2]*+g[a+100>>2];b=a+88|0;x=+g[b>>2];y=v*+g[a+96>>2];z=x+(q-l+w*+g[a+152>>2])*(-0.0- +g[a+188>>2]);A=-0.0-y;B=z<y?z:y;y=B<A?A:B;g[b>>2]=y;B=y-x;x=l-t*B;l=q+u*B;B=+g[a+124>>2];q=+g[a+120>>2];y=+g[a+116>>2];A=+g[a+112>>2];z=+g[a+144>>2]*w+(o+B*(-0.0-l)-j-y*(-0.0-x));C=w*+g[a+148>>2]+(p+q*l-k-A*x);w=+g[a+184>>2]*C+ +g[a+176>>2]*z;b=a+80|0;m=b;D=+g[m>>2];E=+g[m+4>>2];m=b|0;F=D-(+g[a+180>>2]*C+ +g[a+172>>2]*z);g[m>>2]=F;b=a+84|0;z=+g[b>>2]-w;g[b>>2]=z;w=v*+g[a+92>>2];v=F*F+z*z;if(v>w*w){C=+R(v);if(C<1.1920928955078125e-7){G=F;H=z}else{v=1.0/C;C=F*v;g[m>>2]=C;I=z*v;g[b>>2]=I;G=C;H=I}I=w*G;g[m>>2]=I;G=w*H;g[b>>2]=G;J=I;K=G}else{J=F;K=z}z=J-D;D=K-E;E=+(j-r*z);j=+(k-r*D);g[i>>2]=E;g[i+4>>2]=j;i=c[f>>2]|0;g[i+((c[d>>2]|0)*12|0)+8>>2]=x-t*(A*D-z*y);d=i+((c[e>>2]|0)*12|0)|0;y=+(o+s*z);o=+(p+s*D);g[d>>2]=y;g[d+4>>2]=o;g[(c[f>>2]|0)+((c[e>>2]|0)*12|0)+8>>2]=l+u*(D*q-z*B);return}function $f(a,b){a=a|0;b=b|0;return 1}function ag(a,b){a=a|0;b=b|0;var d=0;d=(c[b+48>>2]|0)+12|0;b=a;a=c[d+4>>2]|0;c[b>>2]=c[d>>2];c[b+4>>2]=a;return}function bg(a,b){a=a|0;b=b|0;var d=0;d=(c[b+52>>2]|0)+12|0;b=a;a=c[d+4>>2]|0;c[b>>2]=c[d>>2];c[b+4>>2]=a;return}function cg(a,b,c){a=a|0;b=b|0;c=+c;var d=0.0;d=+g[b+84>>2]*c;g[a>>2]=+g[b+80>>2]*c;g[a+4>>2]=d;return}function dg(a,b){a=a|0;b=+b;return+(+g[a+88>>2]*b)}function eg(a,d){a=a|0;d=d|0;var e=0,f=0,h=0,i=0;e=a+68|0;do{if(!(+g[d>>2]!=+g[e>>2])){if(+g[d+4>>2]!=+g[a+72>>2]){break}return}}while(0);f=c[a+48>>2]|0;h=f+4|0;i=b[h>>1]|0;if((i&2)==0){b[h>>1]=i|2;g[f+160>>2]=0.0}f=c[a+52>>2]|0;a=f+4|0;i=b[a>>1]|0;if((i&2)==0){b[a>>1]=i|2;g[f+160>>2]=0.0}f=d;d=e;e=c[f+4>>2]|0;c[d>>2]=c[f>>2];c[d+4>>2]=e;return}function fg(a,d){a=a|0;d=+d;var e=0,f=0,h=0,i=0;e=a+76|0;if(!(+g[e>>2]!=d)){return}f=c[a+48>>2]|0;h=f+4|0;i=b[h>>1]|0;if((i&2)==0){b[h>>1]=i|2;g[f+160>>2]=0.0}f=c[a+52>>2]|0;a=f+4|0;i=b[a>>1]|0;if((i&2)==0){b[a>>1]=i|2;g[f+160>>2]=0.0}g[e>>2]=d;return}function gg(a){a=a|0;var b=0,e=0,f=0,j=0,k=0.0;b=i;e=c[(c[a+48>>2]|0)+8>>2]|0;f=c[(c[a+52>>2]|0)+8>>2]|0;gn(3960,(j=i,i=i+1|0,i=i+7&-8,c[j>>2]=0,j)|0);i=j;gn(4112,(j=i,i=i+8|0,c[j>>2]=e,j)|0);i=j;gn(3008,(j=i,i=i+8|0,c[j>>2]=f,j)|0);i=j;gn(2632,(j=i,i=i+8|0,c[j>>2]=d[a+61|0]|0,j)|0);i=j;k=+g[a+72>>2];gn(2208,(j=i,i=i+16|0,h[j>>3]=+g[a+68>>2],h[j+8>>3]=k,j)|0);i=j;gn(1744,(j=i,i=i+8|0,h[j>>3]=+g[a+76>>2],j)|0);i=j;gn(1296,(j=i,i=i+8|0,h[j>>3]=+g[a+92>>2],j)|0);i=j;gn(960,(j=i,i=i+8|0,h[j>>3]=+g[a+96>>2],j)|0);i=j;gn(584,(j=i,i=i+8|0,h[j>>3]=+g[a+100>>2],j)|0);i=j;gn(296,(j=i,i=i+8|0,c[j>>2]=c[a+56>>2],j)|0);i=j;i=b;return}function hg(a,b){a=a|0;b=b|0;return}function ig(a){a=a|0;return}function jg(a){a=a|0;Jn(a);return}function kg(a,b,d,e,f){a=a|0;b=b|0;d=d|0;e=e|0;f=f|0;var h=0,i=0.0,j=0,k=0.0,l=0.0,m=0.0,n=0.0,o=0.0;c[a+8>>2]=b;c[a+12>>2]=d;h=e|0;i=+g[h>>2]- +g[b+12>>2];j=e+4|0;k=+g[j>>2]- +g[b+16>>2];l=+g[b+24>>2];m=+g[b+20>>2];b=a+20|0;n=+(i*l+k*m);o=+(l*k+i*(-0.0-m));g[b>>2]=n;g[b+4>>2]=o;b=f|0;o=+g[b>>2]- +g[d+12>>2];e=f+4|0;n=+g[e>>2]- +g[d+16>>2];m=+g[d+24>>2];i=+g[d+20>>2];d=a+28|0;k=+(o*m+n*i);l=+(m*n+o*(-0.0-i));g[d>>2]=k;g[d+4>>2]=l;l=+g[b>>2]- +g[h>>2];k=+g[e>>2]- +g[j>>2];g[a+36>>2]=+R(l*l+k*k);return}function lg(a,b){a=a|0;b=b|0;var d=0,e=0,f=0;Kh(a|0,b|0);c[a>>2]=5184;d=b+20|0;e=a+80|0;f=c[d+4>>2]|0;c[e>>2]=c[d>>2];c[e+4>>2]=f;f=b+28|0;e=a+88|0;d=c[f+4>>2]|0;c[e>>2]=c[f>>2];c[e+4>>2]=d;g[a+104>>2]=+g[b+36>>2];g[a+68>>2]=+g[b+40>>2];g[a+72>>2]=+g[b+44>>2];g[a+100>>2]=0.0;g[a+96>>2]=0.0;g[a+76>>2]=0.0;return}function mg(b,d){b=b|0;d=d|0;var e=0,f=0,h=0,i=0,j=0,l=0,m=0,n=0,o=0,p=0,q=0,r=0,s=0.0,t=0.0,u=0.0,v=0.0,w=0.0,x=0.0,y=0.0,z=0.0,A=0.0,B=0.0,C=0.0,D=0.0,E=0.0,F=0.0,G=0.0,H=0.0,I=0.0,J=0.0,K=0.0,L=0.0,M=0.0,N=0.0,O=0.0,P=0.0,Q=0.0,S=0.0,V=0.0,W=0.0,X=0.0,Y=0.0,Z=0.0,_=0.0,$=0.0,aa=0,ba=0,ca=0,da=0,ea=0,fa=0,ga=0,ha=0,ia=0,ja=0,ka=0,la=0,ma=0,na=0,oa=0,pa=0,qa=0,ra=0,sa=0,ta=0,ua=0,va=0,wa=0,xa=0,ya=0,za=0,Aa=0,Ba=0,Ca=0,Da=0,Ea=0,Fa=0,Ga=0,Ha=0,Ia=0,Ja=0;e=c[b+48>>2]|0;f=c[e+8>>2]|0;h=b+108|0;c[h>>2]=f;i=c[b+52>>2]|0;j=c[i+8>>2]|0;l=b+112|0;c[l>>2]=j;m=e+44|0;n=b+140|0;o=c[m>>2]|0;p=c[m+4>>2]|0;c[n>>2]=o;c[n+4>>2]=p;n=i+44|0;m=b+148|0;q=c[n>>2]|0;r=c[n+4>>2]|0;c[m>>2]=q;c[m+4>>2]=r;s=+g[e+136>>2];g[b+156>>2]=s;t=+g[i+136>>2];g[b+160>>2]=t;u=+g[e+144>>2];g[b+164>>2]=u;v=+g[i+144>>2];g[b+168>>2]=v;i=c[d+28>>2]|0;e=i+(f*12|0)|0;w=+g[e>>2];x=+g[e+4>>2];y=+g[i+(f*12|0)+8>>2];e=d+32|0;m=c[e>>2]|0;n=m+(f*12|0)|0;z=+g[n>>2];A=+g[n+4>>2];B=+g[m+(f*12|0)+8>>2];n=i+(j*12|0)|0;C=+g[n>>2];D=+g[n+4>>2];E=+g[i+(j*12|0)+8>>2];i=m+(j*12|0)|0;F=+g[i>>2];G=+g[i+4>>2];H=+g[m+(j*12|0)+8>>2];I=+U(y);J=+T(y);y=+U(E);K=+T(E);E=+g[b+80>>2]-(c[k>>2]=o,+g[k>>2]);L=+g[b+84>>2]-(c[k>>2]=p,+g[k>>2]);M=J*E-I*L;N=I*E+J*L;p=b+124|0;L=+M;J=+N;g[p>>2]=L;g[p+4>>2]=J;J=+g[b+88>>2]-(c[k>>2]=q,+g[k>>2]);L=+g[b+92>>2]-(c[k>>2]=r,+g[k>>2]);E=K*J-y*L;I=y*J+K*L;r=b+132|0;L=+E;K=+I;g[r>>2]=L;g[r+4>>2]=K;r=b+116|0;K=C+E-w-M;w=D+I-x-N;q=r;x=+K;D=+w;g[q>>2]=x;g[q+4>>2]=D;q=r|0;D=+R(K*K+w*w);if(D>.004999999888241291){x=1.0/D;C=K*x;g[q>>2]=C;O=x*w;P=C}else{g[q>>2]=0.0;O=0.0;P=0.0}g[b+120>>2]=O;C=O*M-N*P;w=O*E-P*I;x=t+(s+C*C*u)+w*w*v;if(x!=0.0){Q=1.0/x}else{Q=0.0}q=b+172|0;g[q>>2]=Q;w=+g[b+68>>2];if(w>0.0){C=D- +g[b+104>>2];D=w*6.2831854820251465;w=D*Q*D;K=+g[d>>2];L=K*(D*Q*2.0*+g[b+72>>2]+w*K);r=b+96|0;g[r>>2]=L;if(L!=0.0){S=1.0/L}else{S=0.0}g[r>>2]=S;g[b+76>>2]=w*C*K*S;K=x+S;if(K!=0.0){V=1.0/K}else{V=0.0}g[q>>2]=V}else{g[b+96>>2]=0.0;g[b+76>>2]=0.0}if((a[d+24|0]|0)==0){g[b+100>>2]=0.0;W=B;X=H;Y=F;Z=G;_=z;$=A;aa=c[e>>2]|0;ba=aa+(f*12|0)|0;ca=ba;da=(g[k>>2]=_,c[k>>2]|0);ea=(g[k>>2]=$,c[k>>2]|0);fa=ea;ga=0;ha=0;ia=fa;ja=da;ka=0;la=ha|ja;ma=ia|ka;na=ca|0;c[na>>2]=la;oa=ca+4|0;c[oa>>2]=ma;pa=c[h>>2]|0;qa=c[e>>2]|0;ra=qa+(pa*12|0)+8|0;g[ra>>2]=W;sa=c[l>>2]|0;ta=qa+(sa*12|0)|0;ua=ta;va=(g[k>>2]=Y,c[k>>2]|0);wa=(g[k>>2]=Z,c[k>>2]|0);xa=wa;ya=0;za=0;Aa=xa;Ba=va;Ca=0;Da=za|Ba;Ea=Aa|Ca;Fa=ua|0;c[Fa>>2]=Da;Ga=ua+4|0;c[Ga>>2]=Ea;Ha=c[l>>2]|0;Ia=c[e>>2]|0;Ja=Ia+(Ha*12|0)+8|0;g[Ja>>2]=X;return}else{q=b+100|0;V=+g[d+8>>2]*+g[q>>2];g[q>>2]=V;K=P*V;P=V*O;W=B-u*(P*M-K*N);X=H+v*(P*E-K*I);Y=F+K*t;Z=G+P*t;_=z-K*s;$=A-P*s;aa=c[e>>2]|0;ba=aa+(f*12|0)|0;ca=ba;da=(g[k>>2]=_,c[k>>2]|0);ea=(g[k>>2]=$,c[k>>2]|0);fa=ea;ga=0;ha=0;ia=fa;ja=da;ka=0;la=ha|ja;ma=ia|ka;na=ca|0;c[na>>2]=la;oa=ca+4|0;c[oa>>2]=ma;pa=c[h>>2]|0;qa=c[e>>2]|0;ra=qa+(pa*12|0)+8|0;g[ra>>2]=W;sa=c[l>>2]|0;ta=qa+(sa*12|0)|0;ua=ta;va=(g[k>>2]=Y,c[k>>2]|0);wa=(g[k>>2]=Z,c[k>>2]|0);xa=wa;ya=0;za=0;Aa=xa;Ba=va;Ca=0;Da=za|Ba;Ea=Aa|Ca;Fa=ua|0;c[Fa>>2]=Da;Ga=ua+4|0;c[Ga>>2]=Ea;Ha=c[l>>2]|0;Ia=c[e>>2]|0;Ja=Ia+(Ha*12|0)+8|0;g[Ja>>2]=X;return}}function ng(a,b){a=a|0;b=b|0;var d=0,e=0,f=0,h=0,i=0.0,j=0.0,k=0.0,l=0,m=0,n=0.0,o=0.0,p=0.0,q=0.0,r=0.0,s=0.0,t=0.0,u=0.0,v=0.0,w=0.0,x=0.0;d=a+108|0;e=c[d>>2]|0;f=b+32|0;b=c[f>>2]|0;h=b+(e*12|0)|0;i=+g[h>>2];j=+g[h+4>>2];k=+g[b+(e*12|0)+8>>2];e=a+112|0;l=c[e>>2]|0;m=b+(l*12|0)|0;n=+g[m>>2];o=+g[m+4>>2];p=+g[b+(l*12|0)+8>>2];q=+g[a+128>>2];r=+g[a+124>>2];s=+g[a+136>>2];t=+g[a+132>>2];u=+g[a+116>>2];v=+g[a+120>>2];l=a+100|0;w=+g[l>>2];x=(+g[a+76>>2]+(u*(n+s*(-0.0-p)-(i+q*(-0.0-k)))+v*(o+p*t-(j+k*r)))+ +g[a+96>>2]*w)*(-0.0- +g[a+172>>2]);g[l>>2]=w+x;w=u*x;u=v*x;x=+g[a+156>>2];v=k- +g[a+164>>2]*(u*r-w*q);q=+g[a+160>>2];r=p+ +g[a+168>>2]*(u*t-w*s);s=+(i-x*w);i=+(j-x*u);g[h>>2]=s;g[h+4>>2]=i;h=c[f>>2]|0;g[h+((c[d>>2]|0)*12|0)+8>>2]=v;d=h+((c[e>>2]|0)*12|0)|0;v=+(n+w*q);w=+(o+u*q);g[d>>2]=v;g[d+4>>2]=w;g[(c[f>>2]|0)+((c[e>>2]|0)*12|0)+8>>2]=r;return}function og(a,b){a=a|0;b=b|0;var d=0,e=0,f=0,h=0,i=0,j=0.0,k=0.0,l=0.0,m=0,n=0,o=0.0,p=0.0,q=0.0,r=0.0,s=0.0,t=0.0,u=0.0,v=0.0,w=0.0,x=0.0,y=0.0,z=0.0,A=0.0,B=0.0,C=0.0;if(+g[a+68>>2]>0.0){d=1;return d|0}e=a+108|0;f=c[e>>2]|0;h=b+28|0;b=c[h>>2]|0;i=b+(f*12|0)|0;j=+g[i>>2];k=+g[i+4>>2];l=+g[b+(f*12|0)+8>>2];f=a+112|0;m=c[f>>2]|0;n=b+(m*12|0)|0;o=+g[n>>2];p=+g[n+4>>2];q=+g[b+(m*12|0)+8>>2];r=+U(l);s=+T(l);t=+U(q);u=+T(q);v=+g[a+80>>2]- +g[a+140>>2];w=+g[a+84>>2]- +g[a+144>>2];x=s*v-r*w;y=r*v+s*w;w=+g[a+88>>2]- +g[a+148>>2];s=+g[a+92>>2]- +g[a+152>>2];v=u*w-t*s;r=t*w+u*s;s=o+v-j-x;u=p+r-k-y;w=+R(s*s+u*u);if(w<1.1920928955078125e-7){z=0.0;A=s;B=u}else{t=1.0/w;z=w;A=s*t;B=u*t}t=z- +g[a+104>>2];z=t<.20000000298023224?t:.20000000298023224;t=z<-.20000000298023224?-.20000000298023224:z;z=t*(-0.0- +g[a+172>>2]);u=A*z;A=B*z;z=+g[a+156>>2];B=l- +g[a+164>>2]*(x*A-y*u);y=+g[a+160>>2];x=q+ +g[a+168>>2]*(v*A-r*u);r=+(j-z*u);j=+(k-z*A);g[i>>2]=r;g[i+4>>2]=j;i=c[h>>2]|0;g[i+((c[e>>2]|0)*12|0)+8>>2]=B;e=i+((c[f>>2]|0)*12|0)|0;B=+(o+y*u);u=+(p+y*A);g[e>>2]=B;g[e+4>>2]=u;g[(c[h>>2]|0)+((c[f>>2]|0)*12|0)+8>>2]=x;if(t>0.0){C=t}else{C=-0.0-t}d=C<.004999999888241291;return d|0}function pg(a,b){a=a|0;b=b|0;var d=0,e=0.0,f=0.0,h=0.0,i=0.0,j=0.0;d=c[b+48>>2]|0;e=+g[d+24>>2];f=+g[b+80>>2];h=+g[d+20>>2];i=+g[b+84>>2];j=f*h+e*i+ +g[d+16>>2];g[a>>2]=+g[d+12>>2]+(e*f-h*i);g[a+4>>2]=j;return}function qg(a,b){a=a|0;b=b|0;var d=0,e=0.0,f=0.0,h=0.0,i=0.0,j=0.0;d=c[b+52>>2]|0;e=+g[d+24>>2];f=+g[b+88>>2];h=+g[d+20>>2];i=+g[b+92>>2];j=f*h+e*i+ +g[d+16>>2];g[a>>2]=+g[d+12>>2]+(e*f-h*i);g[a+4>>2]=j;return}function rg(a,b,c){a=a|0;b=b|0;c=+c;var d=0.0;d=+g[b+100>>2]*c;c=d*+g[b+120>>2];g[a>>2]=+g[b+116>>2]*d;g[a+4>>2]=c;return}function sg(a,b){a=a|0;b=+b;return+0.0}function tg(a){a=a|0;var b=0,e=0,f=0,j=0,k=0.0;b=i;e=c[(c[a+48>>2]|0)+8>>2]|0;f=c[(c[a+52>>2]|0)+8>>2]|0;gn(3688,(j=i,i=i+1|0,i=i+7&-8,c[j>>2]=0,j)|0);i=j;gn(3896,(j=i,i=i+8|0,c[j>>2]=e,j)|0);i=j;gn(2976,(j=i,i=i+8|0,c[j>>2]=f,j)|0);i=j;gn(2592,(j=i,i=i+8|0,c[j>>2]=d[a+61|0]|0,j)|0);i=j;k=+g[a+84>>2];gn(2160,(j=i,i=i+16|0,h[j>>3]=+g[a+80>>2],h[j+8>>3]=k,j)|0);i=j;k=+g[a+92>>2];gn(1696,(j=i,i=i+16|0,h[j>>3]=+g[a+88>>2],h[j+8>>3]=k,j)|0);i=j;gn(1272,(j=i,i=i+8|0,h[j>>3]=+g[a+104>>2],j)|0);i=j;gn(928,(j=i,i=i+8|0,h[j>>3]=+g[a+68>>2],j)|0);i=j;gn(552,(j=i,i=i+8|0,h[j>>3]=+g[a+72>>2],j)|0);i=j;gn(248,(j=i,i=i+8|0,c[j>>2]=c[a+56>>2],j)|0);i=j;i=b;return}function ug(a){a=a|0;return}function vg(a){a=a|0;Jn(a);return}function wg(a,b,d,e){a=a|0;b=b|0;d=d|0;e=e|0;var f=0,h=0.0,i=0,j=0.0,k=0.0,l=0.0,m=0.0,n=0.0;c[a+8>>2]=b;c[a+12>>2]=d;f=e|0;h=+g[f>>2]- +g[b+12>>2];i=e+4|0;j=+g[i>>2]- +g[b+16>>2];k=+g[b+24>>2];l=+g[b+20>>2];b=a+20|0;m=+(h*k+j*l);n=+(k*j+h*(-0.0-l));g[b>>2]=m;g[b+4>>2]=n;n=+g[f>>2]- +g[d+12>>2];m=+g[i>>2]- +g[d+16>>2];l=+g[d+24>>2];h=+g[d+20>>2];d=a+28|0;j=+(n*l+m*h);k=+(l*m+n*(-0.0-h));g[d>>2]=j;g[d+4>>2]=k;return}function xg(a,b){a=a|0;b=b|0;var d=0,e=0,f=0;Kh(a|0,b|0);c[a>>2]=5120;d=b+20|0;e=a+68|0;f=c[d+4>>2]|0;c[e>>2]=c[d>>2];c[e+4>>2]=f;f=b+28|0;e=a+76|0;d=c[f+4>>2]|0;c[e>>2]=c[f>>2];c[e+4>>2]=d;g[a+84>>2]=0.0;g[a+88>>2]=0.0;g[a+92>>2]=0.0;g[a+96>>2]=+g[b+36>>2];g[a+100>>2]=+g[b+40>>2];return}function yg(b,d){b=b|0;d=d|0;var e=0,f=0,h=0,i=0,j=0,l=0,m=0,n=0,o=0,p=0,q=0,r=0,s=0.0,t=0.0,u=0.0,v=0.0,w=0.0,x=0.0,y=0.0,z=0.0,A=0.0,B=0.0,C=0.0,D=0.0,E=0.0,F=0.0,G=0.0,H=0.0,I=0.0,J=0.0,K=0.0,L=0.0,M=0.0,N=0.0,O=0.0,P=0.0,Q=0.0,R=0.0,S=0.0;e=c[b+48>>2]|0;f=c[e+8>>2]|0;h=b+104|0;c[h>>2]=f;i=c[b+52>>2]|0;j=c[i+8>>2]|0;l=b+108|0;c[l>>2]=j;m=e+44|0;n=b+128|0;o=c[m>>2]|0;p=c[m+4>>2]|0;c[n>>2]=o;c[n+4>>2]=p;n=i+44|0;m=b+136|0;q=c[n>>2]|0;r=c[n+4>>2]|0;c[m>>2]=q;c[m+4>>2]=r;s=+g[e+136>>2];g[b+144>>2]=s;t=+g[i+136>>2];g[b+148>>2]=t;u=+g[e+144>>2];g[b+152>>2]=u;v=+g[i+144>>2];g[b+156>>2]=v;i=c[d+28>>2]|0;w=+g[i+(f*12|0)+8>>2];e=d+32|0;m=c[e>>2]|0;n=m+(f*12|0)|0;x=+g[n>>2];y=+g[n+4>>2];z=+g[m+(f*12|0)+8>>2];A=+g[i+(j*12|0)+8>>2];i=m+(j*12|0)|0;B=+g[i>>2];C=+g[i+4>>2];D=+g[m+(j*12|0)+8>>2];E=+U(w);F=+T(w);w=+U(A);G=+T(A);A=+g[b+68>>2]-(c[k>>2]=o,+g[k>>2]);H=+g[b+72>>2]-(c[k>>2]=p,+g[k>>2]);I=F*A-E*H;J=E*A+F*H;p=b+112|0;H=+I;F=+J;g[p>>2]=H;g[p+4>>2]=F;F=+g[b+76>>2]-(c[k>>2]=q,+g[k>>2]);H=+g[b+80>>2]-(c[k>>2]=r,+g[k>>2]);A=G*F-w*H;E=w*F+G*H;r=b+120|0;H=+A;G=+E;g[r>>2]=H;g[r+4>>2]=G;G=s+t;H=G+J*u*J+E*v*E;F=v*A;w=J*I*(-0.0-u)-E*F;K=G+I*u*I+A*F;F=H*K-w*w;if(F!=0.0){L=1.0/F}else{L=F}F=w*(-0.0-L);g[b+160>>2]=K*L;g[b+164>>2]=F;g[b+168>>2]=F;g[b+172>>2]=H*L;L=u+v;if(L>0.0){M=1.0/L}else{M=L}g[b+176>>2]=M;r=b+84|0;if((a[d+24|0]|0)==0){g[r>>2]=0.0;g[b+88>>2]=0.0;g[b+92>>2]=0.0;N=z;O=D;P=B;Q=C;R=x;S=y}else{q=d+8|0;M=+g[q>>2];d=r|0;L=M*+g[d>>2];g[d>>2]=L;d=b+88|0;H=M*+g[d>>2];g[d>>2]=H;d=b+92|0;M=+g[q>>2]*+g[d>>2];g[d>>2]=M;N=z-u*(M+(H*I-L*J));O=D+v*(M+(H*A-L*E));P=B+t*L;Q=C+t*H;R=x-s*L;S=y-s*H}d=(c[e>>2]|0)+(f*12|0)|0;H=+R;R=+S;g[d>>2]=H;g[d+4>>2]=R;d=c[e>>2]|0;g[d+((c[h>>2]|0)*12|0)+8>>2]=N;h=d+((c[l>>2]|0)*12|0)|0;N=+P;P=+Q;g[h>>2]=N;g[h+4>>2]=P;g[(c[e>>2]|0)+((c[l>>2]|0)*12|0)+8>>2]=O;return}function zg(a,b){a=a|0;b=b|0;var d=0,e=0,f=0,h=0,i=0,j=0.0,k=0.0,l=0.0,m=0,n=0,o=0.0,p=0.0,q=0.0,r=0.0,s=0.0,t=0.0,u=0.0,v=0.0,w=0.0,x=0.0,y=0.0,z=0.0,A=0.0,B=0.0,C=0.0,D=0.0,E=0.0,F=0.0,G=0.0,H=0.0,I=0.0,J=0.0,K=0.0;d=a+104|0;e=c[d>>2]|0;f=b+32|0;h=c[f>>2]|0;i=h+(e*12|0)|0;j=+g[i>>2];k=+g[i+4>>2];l=+g[h+(e*12|0)+8>>2];e=a+108|0;m=c[e>>2]|0;n=h+(m*12|0)|0;o=+g[n>>2];p=+g[n+4>>2];q=+g[h+(m*12|0)+8>>2];r=+g[a+144>>2];s=+g[a+148>>2];t=+g[a+152>>2];u=+g[a+156>>2];v=+g[b>>2];b=a+92|0;w=+g[b>>2];x=v*+g[a+100>>2];y=w+(q-l)*(-0.0- +g[a+176>>2]);z=-0.0-x;A=y<x?y:x;x=A<z?z:A;g[b>>2]=x;A=x-w;w=l-t*A;l=q+u*A;A=+g[a+124>>2];q=+g[a+120>>2];x=+g[a+116>>2];z=+g[a+112>>2];y=o+A*(-0.0-l)-j-x*(-0.0-w);B=p+q*l-k-z*w;C=+g[a+172>>2]*B+ +g[a+164>>2]*y;b=a+84|0;m=b;D=+g[m>>2];E=+g[m+4>>2];m=b|0;F=D-(+g[a+168>>2]*B+ +g[a+160>>2]*y);g[m>>2]=F;b=a+88|0;y=+g[b>>2]-C;g[b>>2]=y;C=v*+g[a+96>>2];v=F*F+y*y;if(v>C*C){B=+R(v);if(B<1.1920928955078125e-7){G=F;H=y}else{v=1.0/B;B=F*v;g[m>>2]=B;I=y*v;g[b>>2]=I;G=B;H=I}I=C*G;g[m>>2]=I;G=C*H;g[b>>2]=G;J=I;K=G}else{J=F;K=y}y=J-D;D=K-E;E=+(j-r*y);j=+(k-r*D);g[i>>2]=E;g[i+4>>2]=j;i=c[f>>2]|0;g[i+((c[d>>2]|0)*12|0)+8>>2]=w-t*(z*D-y*x);d=i+((c[e>>2]|0)*12|0)|0;x=+(o+s*y);o=+(p+s*D);g[d>>2]=x;g[d+4>>2]=o;g[(c[f>>2]|0)+((c[e>>2]|0)*12|0)+8>>2]=l+u*(D*q-y*A);return}function Ag(a,b){a=a|0;b=b|0;return 1}function Bg(a,b){a=a|0;b=b|0;var d=0,e=0.0,f=0.0,h=0.0,i=0.0,j=0.0;d=c[b+48>>2]|0;e=+g[d+24>>2];f=+g[b+68>>2];h=+g[d+20>>2];i=+g[b+72>>2];j=f*h+e*i+ +g[d+16>>2];g[a>>2]=+g[d+12>>2]+(e*f-h*i);g[a+4>>2]=j;return}function Cg(a,b){a=a|0;b=b|0;var d=0,e=0.0,f=0.0,h=0.0,i=0.0,j=0.0;d=c[b+52>>2]|0;e=+g[d+24>>2];f=+g[b+76>>2];h=+g[d+20>>2];i=+g[b+80>>2];j=f*h+e*i+ +g[d+16>>2];g[a>>2]=+g[d+12>>2]+(e*f-h*i);g[a+4>>2]=j;return}function Dg(a,b,c){a=a|0;b=b|0;c=+c;var d=0.0;d=+g[b+88>>2]*c;g[a>>2]=+g[b+84>>2]*c;g[a+4>>2]=d;return}function Eg(a,b){a=a|0;b=+b;return+(+g[a+92>>2]*b)}function Fg(a){a=a|0;var b=0,e=0,f=0,j=0,k=0.0;b=i;e=c[(c[a+48>>2]|0)+8>>2]|0;f=c[(c[a+52>>2]|0)+8>>2]|0;gn(3352,(j=i,i=i+1|0,i=i+7&-8,c[j>>2]=0,j)|0);i=j;gn(3752,(j=i,i=i+8|0,c[j>>2]=e,j)|0);i=j;gn(2944,(j=i,i=i+8|0,c[j>>2]=f,j)|0);i=j;gn(2552,(j=i,i=i+8|0,c[j>>2]=d[a+61|0]|0,j)|0);i=j;k=+g[a+72>>2];gn(2112,(j=i,i=i+16|0,h[j>>3]=+g[a+68>>2],h[j+8>>3]=k,j)|0);i=j;k=+g[a+80>>2];gn(1648,(j=i,i=i+16|0,h[j>>3]=+g[a+76>>2],h[j+8>>3]=k,j)|0);i=j;gn(1240,(j=i,i=i+8|0,h[j>>3]=+g[a+96>>2],j)|0);i=j;gn(896,(j=i,i=i+8|0,h[j>>3]=+g[a+100>>2],j)|0);i=j;gn(504,(j=i,i=i+8|0,c[j>>2]=c[a+56>>2],j)|0);i=j;i=b;return}function Gg(a){a=a|0;return}function Hg(a){a=a|0;Jn(a);return}function Ig(a,b,d,e){a=a|0;b=b|0;d=d|0;e=e|0;var f=0,h=0.0,i=0,j=0.0,k=0.0,l=0.0,m=0.0,n=0.0;c[a+8>>2]=b;c[a+12>>2]=d;f=e|0;h=+g[f>>2]- +g[b+12>>2];i=e+4|0;j=+g[i>>2]- +g[b+16>>2];k=+g[b+24>>2];l=+g[b+20>>2];e=a+20|0;m=+(h*k+j*l);n=+(k*j+h*(-0.0-l));g[e>>2]=m;g[e+4>>2]=n;n=+g[f>>2]- +g[d+12>>2];m=+g[i>>2]- +g[d+16>>2];l=+g[d+24>>2];h=+g[d+20>>2];i=a+28|0;j=+(n*l+m*h);k=+(l*m+n*(-0.0-h));g[i>>2]=j;g[i+4>>2]=k;g[a+36>>2]=+g[d+72>>2]- +g[b+72>>2];return}function Jg(a,b){a=a|0;b=b|0;var d=0,e=0,f=0;Kh(a|0,b|0);c[a>>2]=5784;d=b+20|0;e=a+80|0;f=c[d+4>>2]|0;c[e>>2]=c[d>>2];c[e+4>>2]=f;f=b+28|0;e=a+88|0;d=c[f+4>>2]|0;c[e>>2]=c[f>>2];c[e+4>>2]=d;g[a+96>>2]=+g[b+36>>2];g[a+68>>2]=+g[b+40>>2];g[a+72>>2]=+g[b+44>>2];g[a+104>>2]=0.0;g[a+108>>2]=0.0;g[a+112>>2]=0.0;return}function Kg(b,d){b=b|0;d=d|0;var e=0,f=0,h=0,j=0,l=0,m=0,n=0,o=0,p=0,q=0,r=0,s=0,t=0,u=0,v=0.0,w=0.0,x=0.0,y=0.0,z=0.0,A=0.0,B=0.0,C=0.0,D=0.0,E=0.0,F=0.0,G=0.0,H=0.0,I=0.0,J=0.0,K=0.0,L=0.0,M=0.0,N=0.0,O=0.0,P=0.0,Q=0.0,R=0.0,S=0.0,V=0.0,W=0.0,X=0.0,Y=0.0,Z=0.0,_=0,$=0,aa=0,ba=0,ca=0,da=0,ea=0,fa=0,ga=0,ha=0,ia=0,ja=0,ka=0,la=0,ma=0,na=0,oa=0,pa=0,qa=0,ra=0,sa=0,ta=0,ua=0,va=0,wa=0,xa=0,ya=0,za=0,Aa=0,Ba=0,Ca=0,Da=0,Ea=0,Fa=0,Ga=0,Ha=0,Ia=0;e=i;i=i+40|0;f=e|0;h=c[b+48>>2]|0;j=c[h+8>>2]|0;l=b+116|0;c[l>>2]=j;m=c[b+52>>2]|0;n=c[m+8>>2]|0;o=b+120|0;c[o>>2]=n;p=h+44|0;q=b+140|0;r=c[p>>2]|0;s=c[p+4>>2]|0;c[q>>2]=r;c[q+4>>2]=s;q=m+44|0;p=b+148|0;t=c[q>>2]|0;u=c[q+4>>2]|0;c[p>>2]=t;c[p+4>>2]=u;v=+g[h+136>>2];g[b+156>>2]=v;w=+g[m+136>>2];g[b+160>>2]=w;x=+g[h+144>>2];g[b+164>>2]=x;y=+g[m+144>>2];g[b+168>>2]=y;m=c[d+28>>2]|0;z=+g[m+(j*12|0)+8>>2];h=d+32|0;p=c[h>>2]|0;q=p+(j*12|0)|0;A=+g[q>>2];B=+g[q+4>>2];C=+g[p+(j*12|0)+8>>2];D=+g[m+(n*12|0)+8>>2];m=p+(n*12|0)|0;E=+g[m>>2];F=+g[m+4>>2];G=+g[p+(n*12|0)+8>>2];H=+U(z);I=+T(z);J=+U(D);K=+T(D);n=b+124|0;L=+g[b+80>>2]-(c[k>>2]=r,+g[k>>2]);M=+g[b+84>>2]-(c[k>>2]=s,+g[k>>2]);N=I*L-H*M;O=H*L+I*M;s=n;M=+N;I=+O;g[s>>2]=M;g[s+4>>2]=I;s=b+132|0;I=+g[b+88>>2]-(c[k>>2]=t,+g[k>>2]);M=+g[b+92>>2]-(c[k>>2]=u,+g[k>>2]);L=K*I-J*M;H=J*I+K*M;u=s;M=+L;K=+H;g[u>>2]=M;g[u+4>>2]=K;K=v+w;u=b+128|0;t=b+136|0;g[f>>2]=K+x*O*O+y*H*H;M=-0.0-O;r=n|0;n=s|0;O=x*N*M-y*H*L;g[f+12>>2]=O;I=x*M-y*H;g[f+24>>2]=I;g[f+4>>2]=O;g[f+16>>2]=K+x*N*N+y*L*L;K=x*N+y*L;g[f+28>>2]=K;g[f+8>>2]=I;g[f+20>>2]=K;K=x+y;g[f+32>>2]=K;s=b+68|0;p=b+172|0;if(+g[s>>2]>0.0){Zm(f,p);if(K>0.0){P=1.0/K}else{P=0.0}I=D-z- +g[b+96>>2];z=+g[s>>2]*6.2831854820251465;D=z*P*z;L=+g[d>>2];N=L*(z*P*2.0*+g[b+72>>2]+L*D);s=b+100|0;g[s>>2]=N;if(N!=0.0){Q=1.0/N}else{Q=0.0}g[s>>2]=Q;g[b+76>>2]=I*L*D*Q;D=K+Q;if(D!=0.0){R=1.0/D}else{R=0.0}g[b+204>>2]=R}else{_m(f,p);g[b+100>>2]=0.0;g[b+76>>2]=0.0}p=b+104|0;if((a[d+24|0]|0)==0){g[p>>2]=0.0;g[b+108>>2]=0.0;g[b+112>>2]=0.0;S=C;V=G;W=E;X=F;Y=A;Z=B;_=c[l>>2]|0;$=c[h>>2]|0;aa=$+(_*12|0)|0;ba=aa;ca=(g[k>>2]=Y,c[k>>2]|0);da=(g[k>>2]=Z,c[k>>2]|0);ea=da;fa=0;ga=0;ha=ea;ia=ca;ja=0;ka=ga|ia;la=ha|ja;ma=ba|0;c[ma>>2]=ka;na=ba+4|0;c[na>>2]=la;oa=c[l>>2]|0;pa=c[h>>2]|0;qa=pa+(oa*12|0)+8|0;g[qa>>2]=S;ra=c[o>>2]|0;sa=pa+(ra*12|0)|0;ta=sa;ua=(g[k>>2]=W,c[k>>2]|0);va=(g[k>>2]=X,c[k>>2]|0);wa=va;xa=0;ya=0;za=wa;Aa=ua;Ba=0;Ca=ya|Aa;Da=za|Ba;Ea=ta|0;c[Ea>>2]=Ca;Fa=ta+4|0;c[Fa>>2]=Da;Ga=c[o>>2]|0;Ha=c[h>>2]|0;Ia=Ha+(Ga*12|0)+8|0;g[Ia>>2]=V;i=e;return}else{R=+g[d+8>>2];d=p|0;D=R*+g[d>>2];g[d>>2]=D;d=b+108|0;Q=R*+g[d>>2];g[d>>2]=Q;d=b+112|0;K=R*+g[d>>2];g[d>>2]=K;S=C-x*(K+(Q*+g[r>>2]-D*+g[u>>2]));V=G+y*(K+(Q*+g[n>>2]-D*+g[t>>2]));W=E+w*D;X=F+w*Q;Y=A-v*D;Z=B-v*Q;_=c[l>>2]|0;$=c[h>>2]|0;aa=$+(_*12|0)|0;ba=aa;ca=(g[k>>2]=Y,c[k>>2]|0);da=(g[k>>2]=Z,c[k>>2]|0);ea=da;fa=0;ga=0;ha=ea;ia=ca;ja=0;ka=ga|ia;la=ha|ja;ma=ba|0;c[ma>>2]=ka;na=ba+4|0;c[na>>2]=la;oa=c[l>>2]|0;pa=c[h>>2]|0;qa=pa+(oa*12|0)+8|0;g[qa>>2]=S;ra=c[o>>2]|0;sa=pa+(ra*12|0)|0;ta=sa;ua=(g[k>>2]=W,c[k>>2]|0);va=(g[k>>2]=X,c[k>>2]|0);wa=va;xa=0;ya=0;za=wa;Aa=ua;Ba=0;Ca=ya|Aa;Da=za|Ba;Ea=ta|0;c[Ea>>2]=Ca;Fa=ta+4|0;c[Fa>>2]=Da;Ga=c[o>>2]|0;Ha=c[h>>2]|0;Ia=Ha+(Ga*12|0)+8|0;g[Ia>>2]=V;i=e;return}}function Lg(a,b){a=a|0;b=b|0;var d=0,e=0,f=0,h=0,i=0.0,j=0.0,k=0.0,l=0,m=0,n=0.0,o=0.0,p=0.0,q=0.0,r=0.0,s=0.0,t=0.0,u=0.0,v=0.0,w=0.0,x=0.0,y=0.0,z=0.0,A=0.0,B=0.0,C=0.0,D=0.0,E=0.0,F=0.0,G=0.0,H=0.0;d=a+116|0;e=c[d>>2]|0;f=b+32|0;b=c[f>>2]|0;h=b+(e*12|0)|0;i=+g[h>>2];j=+g[h+4>>2];k=+g[b+(e*12|0)+8>>2];e=a+120|0;l=c[e>>2]|0;m=b+(l*12|0)|0;n=+g[m>>2];o=+g[m+4>>2];p=+g[b+(l*12|0)+8>>2];q=+g[a+156>>2];r=+g[a+160>>2];s=+g[a+164>>2];t=+g[a+168>>2];if(+g[a+68>>2]>0.0){l=a+112|0;u=+g[l>>2];v=(p-k+ +g[a+76>>2]+ +g[a+100>>2]*u)*(-0.0- +g[a+204>>2]);g[l>>2]=u+v;u=k-s*v;w=p+t*v;v=+g[a+136>>2];x=+g[a+132>>2];y=+g[a+128>>2];z=+g[a+124>>2];A=n+v*(-0.0-w)-i-y*(-0.0-u);B=o+x*w-j-z*u;C=+g[a+184>>2]*B+ +g[a+172>>2]*A;D=+g[a+188>>2]*B+ +g[a+176>>2]*A;A=-0.0-C;B=-0.0-D;l=a+104|0;g[l>>2]=+g[l>>2]-C;l=a+108|0;g[l>>2]=+g[l>>2]-D;E=u-s*(z*B-y*A);F=w+t*(x*B-v*A);G=A;H=B}else{B=+g[a+136>>2];A=+g[a+132>>2];v=+g[a+128>>2];x=+g[a+124>>2];w=n+B*(-0.0-p)-i-v*(-0.0-k);y=o+p*A-j-k*x;z=p-k;u=w*+g[a+172>>2]+y*+g[a+184>>2]+z*+g[a+196>>2];D=w*+g[a+176>>2]+y*+g[a+188>>2]+z*+g[a+200>>2];C=w*+g[a+180>>2]+y*+g[a+192>>2]+z*+g[a+204>>2];z=-0.0-u;y=-0.0-D;l=a+104|0;g[l>>2]=+g[l>>2]-u;l=a+108|0;g[l>>2]=+g[l>>2]-D;l=a+112|0;g[l>>2]=+g[l>>2]-C;E=k-s*(x*y-v*z-C);F=p+t*(A*y-B*z-C);G=z;H=y}y=+(i-q*G);i=+(j-q*H);g[h>>2]=y;g[h+4>>2]=i;h=c[f>>2]|0;g[h+((c[d>>2]|0)*12|0)+8>>2]=E;d=h+((c[e>>2]|0)*12|0)|0;E=+(n+r*G);G=+(o+r*H);g[d>>2]=E;g[d+4>>2]=G;g[(c[f>>2]|0)+((c[e>>2]|0)*12|0)+8>>2]=F;return}function Mg(a,b){a=a|0;b=b|0;var d=0,e=0,f=0,h=0,j=0,k=0,l=0,m=0,n=0,o=0,p=0.0,q=0.0,r=0.0,s=0,t=0.0,u=0.0,v=0.0,w=0.0,x=0.0,y=0.0,z=0.0,A=0.0,B=0.0,C=0.0,D=0.0,E=0.0,F=0.0,G=0.0,H=0.0,I=0.0,J=0.0,K=0.0,L=0.0,M=0.0,N=0.0,O=0.0,P=0.0,Q=0;d=i;i=i+88|0;e=d|0;f=d+40|0;h=d+48|0;j=d+56|0;k=d+72|0;l=a+116|0;m=c[l>>2]|0;n=b+28|0;b=c[n>>2]|0;o=b+(m*12|0)|0;p=+g[o>>2];q=+g[o+4>>2];r=+g[b+(m*12|0)+8>>2];m=a+120|0;o=c[m>>2]|0;s=b+(o*12|0)|0;t=+g[s>>2];u=+g[s+4>>2];v=+g[b+(o*12|0)+8>>2];w=+U(r);x=+T(r);y=+U(v);z=+T(v);A=+g[a+156>>2];B=+g[a+160>>2];C=+g[a+164>>2];D=+g[a+168>>2];E=+g[a+80>>2]- +g[a+140>>2];F=+g[a+84>>2]- +g[a+144>>2];G=x*E-w*F;H=w*E+x*F;F=+g[a+88>>2]- +g[a+148>>2];x=+g[a+92>>2]- +g[a+152>>2];E=z*F-y*x;w=y*F+z*x;x=A+B;g[e>>2]=x+C*H*H+D*w*w;z=-0.0-H;F=C*G*z-D*w*E;g[e+12>>2]=F;y=C*z-D*w;g[e+24>>2]=y;g[e+4>>2]=F;g[e+16>>2]=x+C*G*G+D*E*E;x=C*G+D*E;g[e+28>>2]=x;g[e+8>>2]=y;g[e+20>>2]=x;g[e+32>>2]=C+D;x=t+E-p-G;y=u+w-q-H;if(+g[a+68>>2]>0.0){g[f>>2]=x;g[f+4>>2]=y;F=+R(y*y+x*x);Ym(h,e,f);z=-0.0- +g[h>>2];I=-0.0- +g[h+4>>2];J=G*I-H*z;K=0.0;L=F;M=E*I-w*z;N=z;O=I}else{I=v-r- +g[a+96>>2];z=+R(x*x+y*y);if(I>0.0){P=I}else{P=-0.0-I}g[j>>2]=x;g[j+4>>2]=y;g[j+8>>2]=I;Xm(k,e,j);I=-0.0- +g[k>>2];y=-0.0- +g[k+4>>2];x=+g[k+8>>2];J=G*y-H*I-x;K=P;L=z;M=E*y-w*I-x;N=I;O=y}k=(c[n>>2]|0)+((c[l>>2]|0)*12|0)|0;y=+(p-A*N);p=+(q-A*O);g[k>>2]=y;g[k+4>>2]=p;k=c[n>>2]|0;g[k+((c[l>>2]|0)*12|0)+8>>2]=r-C*J;l=k+((c[m>>2]|0)*12|0)|0;J=+(t+B*N);N=+(u+B*O);g[l>>2]=J;g[l+4>>2]=N;g[(c[n>>2]|0)+((c[m>>2]|0)*12|0)+8>>2]=v+D*M;if(L>.004999999888241291){Q=0;i=d;return Q|0}Q=K<=.03490658849477768;i=d;return Q|0}function Ng(a,b){a=a|0;b=b|0;var d=0,e=0.0,f=0.0,h=0.0,i=0.0,j=0.0;d=c[b+48>>2]|0;e=+g[d+24>>2];f=+g[b+80>>2];h=+g[d+20>>2];i=+g[b+84>>2];j=f*h+e*i+ +g[d+16>>2];g[a>>2]=+g[d+12>>2]+(e*f-h*i);g[a+4>>2]=j;return}function Og(a,b){a=a|0;b=b|0;var d=0,e=0.0,f=0.0,h=0.0,i=0.0,j=0.0;d=c[b+52>>2]|0;e=+g[d+24>>2];f=+g[b+88>>2];h=+g[d+20>>2];i=+g[b+92>>2];j=f*h+e*i+ +g[d+16>>2];g[a>>2]=+g[d+12>>2]+(e*f-h*i);g[a+4>>2]=j;return}function Pg(a,b,c){a=a|0;b=b|0;c=+c;var d=0.0;d=+g[b+108>>2]*c;g[a>>2]=+g[b+104>>2]*c;g[a+4>>2]=d;return}function Qg(a,b){a=a|0;b=+b;return+(+g[a+112>>2]*b)}function Rg(a){a=a|0;var b=0,e=0,f=0,j=0,k=0.0;b=i;e=c[(c[a+48>>2]|0)+8>>2]|0;f=c[(c[a+52>>2]|0)+8>>2]|0;gn(3248,(j=i,i=i+1|0,i=i+7&-8,c[j>>2]=0,j)|0);i=j;gn(3720,(j=i,i=i+8|0,c[j>>2]=e,j)|0);i=j;gn(2912,(j=i,i=i+8|0,c[j>>2]=f,j)|0);i=j;gn(2512,(j=i,i=i+8|0,c[j>>2]=d[a+61|0]|0,j)|0);i=j;k=+g[a+84>>2];gn(2064,(j=i,i=i+16|0,h[j>>3]=+g[a+80>>2],h[j+8>>3]=k,j)|0);i=j;k=+g[a+92>>2];gn(1600,(j=i,i=i+16|0,h[j>>3]=+g[a+88>>2],h[j+8>>3]=k,j)|0);i=j;gn(1208,(j=i,i=i+8|0,h[j>>3]=+g[a+96>>2],j)|0);i=j;gn(864,(j=i,i=i+8|0,h[j>>3]=+g[a+68>>2],j)|0);i=j;gn(472,(j=i,i=i+8|0,h[j>>3]=+g[a+72>>2],j)|0);i=j;gn(200,(j=i,i=i+8|0,c[j>>2]=c[a+56>>2],j)|0);i=j;i=b;return}function Sg(a){a=a|0;return}function Tg(a){a=a|0;Jn(a);return}function Ug(a,b){a=a|0;b=b|0;var d=0,e=0,f=0,h=0,i=0,j=0,l=0,m=0,n=0,o=0,p=0,q=0,r=0,s=0,t=0.0,u=0.0,v=0.0,w=0.0,x=0.0,y=0.0,z=0,A=0.0,B=0.0,C=0.0,D=0.0,E=0.0,F=0.0,G=0.0,H=0.0,I=0.0,J=0,K=0.0,L=0,M=0.0,N=0.0,O=0,P=0;Kh(a|0,b|0);c[a>>2]=5912;d=a+92|0;e=a+100|0;f=a+108|0;h=a+116|0;i=a+124|0;j=a+132|0;l=b+20|0;m=c[l>>2]|0;c[a+68>>2]=m;n=b+24|0;o=c[n>>2]|0;c[a+72>>2]=o;p=c[m+4>>2]|0;c[a+76>>2]=p;q=c[o+4>>2]|0;c[a+80>>2]=q;r=c[m+48>>2]|0;c[a+84>>2]=r;s=c[m+52>>2]|0;c[a+48>>2]=s;t=+g[s+20>>2];u=+g[s+24>>2];v=+g[r+20>>2];w=+g[r+24>>2];m=c[l>>2]|0;if((p|0)==1){x=+g[s+72>>2];y=+g[r+72>>2];p=m+68|0;l=f;z=c[p+4>>2]|0;c[l>>2]=c[p>>2];c[l+4>>2]=z;z=m+76|0;l=d;p=c[z+4>>2]|0;c[l>>2]=c[z>>2];c[l+4>>2]=p;A=+g[m+116>>2];g[a+140>>2]=A;g[i>>2]=0.0;g[a+128>>2]=0.0;B=x-y-A}else{A=+g[r+16>>2];y=+g[r+12>>2];x=+g[s+16>>2];C=+g[s+12>>2];s=m+68|0;r=f;f=c[s>>2]|0;p=c[s+4>>2]|0;c[r>>2]=f;c[r+4>>2]=p;r=m+76|0;s=d;d=c[r>>2]|0;l=c[r+4>>2]|0;c[s>>2]=d;c[s+4>>2]=l;g[a+140>>2]=+g[m+100>>2];s=m+84|0;m=i;i=c[s>>2]|0;r=c[s+4>>2]|0;c[m>>2]=i;c[m+4>>2]=r;D=(c[k>>2]=f,+g[k>>2]);E=(c[k>>2]=p,+g[k>>2]);F=(c[k>>2]=d,+g[k>>2]);G=(c[k>>2]=l,+g[k>>2]);H=C-y+(u*F-t*G);y=x-A+(t*F+u*G);G=(c[k>>2]=i,+g[k>>2])*(w*H+v*y-D);B=G+(c[k>>2]=r,+g[k>>2])*(H*(-0.0-v)+w*y-E)}r=c[o+48>>2]|0;c[a+88>>2]=r;i=c[o+52>>2]|0;c[a+52>>2]=i;E=+g[i+20>>2];y=+g[i+24>>2];w=+g[r+20>>2];v=+g[r+24>>2];o=c[n>>2]|0;if((q|0)==1){H=+g[i+72>>2];G=+g[r+72>>2];q=o+68|0;n=h;l=c[q+4>>2]|0;c[n>>2]=c[q>>2];c[n+4>>2]=l;l=o+76|0;n=e;q=c[l+4>>2]|0;c[n>>2]=c[l>>2];c[n+4>>2]=q;D=+g[o+116>>2];g[a+144>>2]=D;g[j>>2]=0.0;g[a+136>>2]=0.0;I=H-G-D;J=b+28|0;K=+g[J>>2];L=a+152|0;g[L>>2]=K;M=I*K;N=B+M;O=a+148|0;g[O>>2]=N;P=a+156|0;g[P>>2]=0.0;return}else{D=+g[r+16>>2];G=+g[r+12>>2];H=+g[i+16>>2];u=+g[i+12>>2];i=o+68|0;r=h;h=c[i>>2]|0;q=c[i+4>>2]|0;c[r>>2]=h;c[r+4>>2]=q;r=o+76|0;i=e;e=c[r>>2]|0;n=c[r+4>>2]|0;c[i>>2]=e;c[i+4>>2]=n;g[a+144>>2]=+g[o+100>>2];i=o+84|0;o=j;j=c[i>>2]|0;r=c[i+4>>2]|0;c[o>>2]=j;c[o+4>>2]=r;F=(c[k>>2]=h,+g[k>>2]);t=(c[k>>2]=q,+g[k>>2]);A=(c[k>>2]=e,+g[k>>2]);x=(c[k>>2]=n,+g[k>>2]);C=u-G+(y*A-E*x);G=H-D+(E*A+y*x);x=(c[k>>2]=j,+g[k>>2])*(v*C+w*G-F);I=x+(c[k>>2]=r,+g[k>>2])*(C*(-0.0-w)+v*G-t);J=b+28|0;K=+g[J>>2];L=a+152|0;g[L>>2]=K;M=I*K;N=B+M;O=a+148|0;g[O>>2]=N;P=a+156|0;g[P>>2]=0.0;return}}function Vg(b,d){b=b|0;d=d|0;var e=0,f=0,h=0,i=0,j=0,l=0,m=0,n=0,o=0,p=0,q=0,r=0,s=0,t=0,u=0,v=0,w=0,x=0,y=0,z=0,A=0,B=0,C=0.0,D=0.0,E=0.0,F=0.0,G=0.0,H=0.0,I=0.0,J=0.0,K=0.0,L=0.0,M=0.0,N=0.0,O=0.0,P=0.0,Q=0.0,R=0.0,S=0.0,V=0.0,W=0.0,X=0.0,Y=0.0,Z=0.0,_=0.0,$=0.0,aa=0.0,ba=0.0,ca=0.0,da=0.0,ea=0.0,fa=0.0,ga=0.0,ha=0.0,ia=0.0,ja=0.0,ka=0.0,la=0.0,ma=0.0,na=0.0,oa=0.0,pa=0.0,qa=0.0,ra=0.0,sa=0.0,ta=0.0,ua=0.0,va=0.0,wa=0.0,xa=0.0,ya=0.0,za=0.0,Aa=0.0,Ba=0.0,Ca=0.0,Da=0.0,Ea=0.0,Fa=0.0,Ga=0.0,Ha=0.0,Ia=0.0,Ja=0.0,Ka=0.0,La=0.0,Ma=0.0;e=c[b+48>>2]|0;f=c[e+8>>2]|0;h=b+160|0;c[h>>2]=f;i=c[b+52>>2]|0;j=c[i+8>>2]|0;l=b+164|0;c[l>>2]=j;m=c[b+84>>2]|0;n=c[m+8>>2]|0;o=b+168|0;c[o>>2]=n;p=c[b+88>>2]|0;q=c[p+8>>2]|0;r=b+172|0;c[r>>2]=q;s=e+44|0;t=b+176|0;u=c[s>>2]|0;v=c[s+4>>2]|0;c[t>>2]=u;c[t+4>>2]=v;t=i+44|0;s=b+184|0;w=c[t>>2]|0;x=c[t+4>>2]|0;c[s>>2]=w;c[s+4>>2]=x;s=m+44|0;t=b+192|0;y=c[s>>2]|0;z=c[s+4>>2]|0;c[t>>2]=y;c[t+4>>2]=z;t=p+44|0;s=b+200|0;A=c[t>>2]|0;B=c[t+4>>2]|0;c[s>>2]=A;c[s+4>>2]=B;C=+g[e+136>>2];g[b+208>>2]=C;D=+g[i+136>>2];g[b+212>>2]=D;E=+g[m+136>>2];g[b+216>>2]=E;F=+g[p+136>>2];g[b+220>>2]=F;G=+g[e+144>>2];g[b+224>>2]=G;H=+g[i+144>>2];g[b+228>>2]=H;I=+g[m+144>>2];g[b+232>>2]=I;J=+g[p+144>>2];g[b+236>>2]=J;p=c[d+28>>2]|0;K=+g[p+(f*12|0)+8>>2];m=d+32|0;i=c[m>>2]|0;e=i+(f*12|0)|0;L=+g[e>>2];M=+g[e+4>>2];N=+g[i+(f*12|0)+8>>2];O=+g[p+(j*12|0)+8>>2];e=i+(j*12|0)|0;P=+g[e>>2];Q=+g[e+4>>2];R=+g[i+(j*12|0)+8>>2];S=+g[p+(n*12|0)+8>>2];j=i+(n*12|0)|0;V=+g[j>>2];W=+g[j+4>>2];X=+g[i+(n*12|0)+8>>2];Y=+g[p+(q*12|0)+8>>2];p=i+(q*12|0)|0;Z=+g[p>>2];_=+g[p+4>>2];$=+g[i+(q*12|0)+8>>2];aa=+U(K);ba=+T(K);K=+U(O);ca=+T(O);O=+U(S);da=+T(S);S=+U(Y);ea=+T(Y);q=b+272|0;g[q>>2]=0.0;Y=(c[k>>2]=A,+g[k>>2]);fa=(c[k>>2]=B,+g[k>>2]);ga=(c[k>>2]=w,+g[k>>2]);ha=(c[k>>2]=x,+g[k>>2]);if((c[b+76>>2]|0)==1){g[b+240>>2]=0.0;g[b+244>>2]=0.0;g[b+256>>2]=1.0;g[b+264>>2]=1.0;ia=G+I;ja=0.0;ka=0.0;la=1.0;ma=1.0}else{na=(c[k>>2]=v,+g[k>>2]);oa=(c[k>>2]=u,+g[k>>2]);pa=(c[k>>2]=z,+g[k>>2]);qa=+g[b+124>>2];ra=+g[b+128>>2];sa=da*qa-O*ra;ta=O*qa+da*ra;ra=+g[b+108>>2]-(c[k>>2]=y,+g[k>>2]);qa=+g[b+112>>2]-pa;pa=+g[b+92>>2]-oa;oa=+g[b+96>>2]-na;y=b+240|0;na=+sa;ua=+ta;g[y>>2]=na;g[y+4>>2]=ua;ua=ta*(da*ra-O*qa)-sa*(O*ra+da*qa);g[b+264>>2]=ua;qa=ta*(ba*pa-aa*oa)-sa*(aa*pa+ba*oa);g[b+256>>2]=qa;ia=E+C+ua*I*ua+qa*G*qa;ja=sa;ka=ta;la=qa;ma=ua}ua=ia+0.0;g[q>>2]=ua;if((c[b+80>>2]|0)==1){g[b+248>>2]=0.0;g[b+252>>2]=0.0;ia=+g[b+152>>2];g[b+260>>2]=ia;g[b+268>>2]=ia;va=ia*ia*(H+J);wa=0.0;xa=0.0;ya=ia;za=ia}else{ia=+g[b+132>>2];qa=+g[b+136>>2];ta=ea*ia-S*qa;sa=S*ia+ea*qa;qa=+g[b+116>>2]-Y;Y=+g[b+120>>2]-fa;fa=+g[b+100>>2]-ga;ga=+g[b+104>>2]-ha;ha=+g[b+152>>2];ia=ta*ha;oa=sa*ha;y=b+248|0;ba=+ia;pa=+oa;g[y>>2]=ba;g[y+4>>2]=pa;pa=(sa*(ea*qa-S*Y)-ta*(S*qa+ea*Y))*ha;g[b+268>>2]=pa;Y=ha*(sa*(ca*fa-K*ga)-ta*(K*fa+ca*ga));g[b+260>>2]=Y;va=ha*ha*(F+D)+pa*J*pa+Y*Y*H;wa=ia;xa=oa;ya=Y;za=pa}pa=ua+va;g[q>>2]=pa;if(pa>0.0){Aa=1.0/pa}else{Aa=0.0}g[q>>2]=Aa;q=b+156|0;if((a[d+24|0]|0)==0){g[q>>2]=0.0;Ba=$;Ca=N;Da=X;Ea=R;Fa=Z;Ga=_;Ha=V;Ia=W;Ja=P;Ka=Q;La=L;Ma=M}else{Aa=+g[q>>2];pa=C*Aa;C=Aa*D;D=Aa*E;E=Aa*F;Ba=$-Aa*J*za;Ca=N+Aa*G*la;Da=X-Aa*I*ma;Ea=R+Aa*H*ya;Fa=Z-wa*E;Ga=_-xa*E;Ha=V-ja*D;Ia=W-ka*D;Ja=P+wa*C;Ka=Q+C*xa;La=L+ja*pa;Ma=M+pa*ka}q=(c[m>>2]|0)+(f*12|0)|0;ka=+La;La=+Ma;g[q>>2]=ka;g[q+4>>2]=La;q=c[m>>2]|0;g[q+((c[h>>2]|0)*12|0)+8>>2]=Ca;h=q+((c[l>>2]|0)*12|0)|0;Ca=+Ja;Ja=+Ka;g[h>>2]=Ca;g[h+4>>2]=Ja;h=c[m>>2]|0;g[h+((c[l>>2]|0)*12|0)+8>>2]=Ea;l=h+((c[o>>2]|0)*12|0)|0;Ea=+Ha;Ha=+Ia;g[l>>2]=Ea;g[l+4>>2]=Ha;l=c[m>>2]|0;g[l+((c[o>>2]|0)*12|0)+8>>2]=Da;o=l+((c[r>>2]|0)*12|0)|0;Da=+Fa;Fa=+Ga;g[o>>2]=Da;g[o+4>>2]=Fa;g[(c[m>>2]|0)+((c[r>>2]|0)*12|0)+8>>2]=Ba;return}function Wg(a,b){a=a|0;b=b|0;var d=0,e=0,f=0,h=0,i=0.0,j=0.0,k=0.0,l=0,m=0,n=0.0,o=0.0,p=0.0,q=0,r=0.0,s=0.0,t=0.0,u=0,v=0.0,w=0.0,x=0.0,y=0.0,z=0.0,A=0.0,B=0.0,C=0.0,D=0.0,E=0.0,F=0.0,G=0.0,H=0.0,I=0.0;d=a+160|0;e=c[d>>2]|0;f=b+32|0;b=c[f>>2]|0;h=b+(e*12|0)|0;i=+g[h>>2];j=+g[h+4>>2];k=+g[b+(e*12|0)+8>>2];e=a+164|0;l=c[e>>2]|0;m=b+(l*12|0)|0;n=+g[m>>2];o=+g[m+4>>2];p=+g[b+(l*12|0)+8>>2];l=a+168|0;m=c[l>>2]|0;q=b+(m*12|0)|0;r=+g[q>>2];s=+g[q+4>>2];t=+g[b+(m*12|0)+8>>2];m=a+172|0;q=c[m>>2]|0;u=b+(q*12|0)|0;v=+g[u>>2];w=+g[u+4>>2];x=+g[b+(q*12|0)+8>>2];y=+g[a+240>>2];z=+g[a+244>>2];A=+g[a+248>>2];B=+g[a+252>>2];C=+g[a+256>>2];D=+g[a+264>>2];E=+g[a+260>>2];F=+g[a+268>>2];G=((i-r)*y+(j-s)*z+((n-v)*A+(o-w)*B)+(k*C-t*D+(p*E-x*F)))*(-0.0- +g[a+272>>2]);q=a+156|0;g[q>>2]=+g[q>>2]+G;H=+g[a+208>>2]*G;I=k+G*+g[a+224>>2]*C;C=G*+g[a+212>>2];k=p+G*+g[a+228>>2]*E;E=G*+g[a+216>>2];p=t-G*+g[a+232>>2]*D;D=G*+g[a+220>>2];t=x-G*+g[a+236>>2]*F;F=+(i+y*H);i=+(j+z*H);g[h>>2]=F;g[h+4>>2]=i;h=c[f>>2]|0;g[h+((c[d>>2]|0)*12|0)+8>>2]=I;d=h+((c[e>>2]|0)*12|0)|0;I=+(n+A*C);n=+(o+C*B);g[d>>2]=I;g[d+4>>2]=n;d=c[f>>2]|0;g[d+((c[e>>2]|0)*12|0)+8>>2]=k;e=d+((c[l>>2]|0)*12|0)|0;k=+(r-y*E);y=+(s-z*E);g[e>>2]=k;g[e+4>>2]=y;e=c[f>>2]|0;g[e+((c[l>>2]|0)*12|0)+8>>2]=p;l=e+((c[m>>2]|0)*12|0)|0;p=+(v-A*D);A=+(w-B*D);g[l>>2]=p;g[l+4>>2]=A;g[(c[f>>2]|0)+((c[m>>2]|0)*12|0)+8>>2]=t;return}function Xg(a,b){a=a|0;b=b|0;var d=0,e=0,f=0,h=0,i=0.0,j=0.0,k=0.0,l=0,m=0,n=0.0,o=0.0,p=0.0,q=0,r=0.0,s=0.0,t=0.0,u=0,v=0.0,w=0.0,x=0.0,y=0.0,z=0.0,A=0.0,B=0.0,C=0.0,D=0.0,E=0.0,F=0.0,G=0.0,H=0.0,I=0.0,J=0.0,K=0.0,L=0.0,M=0.0,N=0.0,O=0.0,P=0.0,Q=0.0,R=0.0,S=0.0,V=0.0,W=0.0,X=0.0,Y=0.0,Z=0.0,_=0.0,$=0.0,aa=0.0,ba=0.0,ca=0.0,da=0.0,ea=0.0,fa=0.0,ga=0.0,ha=0.0,ia=0.0;d=a+160|0;e=c[d>>2]|0;f=b+28|0;b=c[f>>2]|0;h=b+(e*12|0)|0;i=+g[h>>2];j=+g[h+4>>2];k=+g[b+(e*12|0)+8>>2];e=a+164|0;l=c[e>>2]|0;m=b+(l*12|0)|0;n=+g[m>>2];o=+g[m+4>>2];p=+g[b+(l*12|0)+8>>2];l=a+168|0;m=c[l>>2]|0;q=b+(m*12|0)|0;r=+g[q>>2];s=+g[q+4>>2];t=+g[b+(m*12|0)+8>>2];m=a+172|0;q=c[m>>2]|0;u=b+(q*12|0)|0;v=+g[u>>2];w=+g[u+4>>2];x=+g[b+(q*12|0)+8>>2];y=+U(k);z=+T(k);A=+U(p);B=+T(p);C=+U(t);D=+T(t);E=+U(x);F=+T(x);if((c[a+76>>2]|0)==1){G=+g[a+224>>2];H=+g[a+232>>2];I=G+H;J=1.0;K=1.0;L=k-t- +g[a+140>>2];M=0.0;N=0.0;O=G;P=H}else{H=+g[a+124>>2];G=+g[a+128>>2];Q=D*H-C*G;R=C*H+D*G;S=+g[a+108>>2]- +g[a+192>>2];V=+g[a+112>>2]- +g[a+196>>2];W=+g[a+92>>2]- +g[a+176>>2];X=+g[a+96>>2]- +g[a+180>>2];Y=z*W-y*X;Z=y*W+z*X;X=R*(D*S-C*V)-Q*(C*S+D*V);z=R*Y-Q*Z;W=+g[a+232>>2];y=+g[a+224>>2];_=i-r+Y;Y=j-s+Z;I=+g[a+216>>2]+ +g[a+208>>2]+X*X*W+z*y*z;J=X;K=z;L=H*(D*_+C*Y-S)+G*(_*(-0.0-C)+D*Y-V);M=Q;N=R;O=y;P=W}if((c[a+80>>2]|0)==1){W=+g[a+152>>2];y=+g[a+228>>2];R=+g[a+236>>2];$=W*W*(y+R);aa=W;ba=W;ca=p-x- +g[a+144>>2];da=0.0;ea=0.0;fa=W;ga=y;ha=R}else{R=+g[a+132>>2];y=+g[a+136>>2];W=F*R-E*y;Q=E*R+F*y;V=+g[a+116>>2]- +g[a+200>>2];Y=+g[a+120>>2]- +g[a+204>>2];D=+g[a+100>>2]- +g[a+184>>2];C=+g[a+104>>2]- +g[a+188>>2];_=B*D-A*C;G=A*D+B*C;C=+g[a+152>>2];B=C*(Q*(F*V-E*Y)-W*(E*V+F*Y));D=C*(Q*_-W*G);A=+g[a+236>>2];S=+g[a+228>>2];H=n-v+_;_=o-w+G;$=C*C*(+g[a+220>>2]+ +g[a+212>>2])+B*B*A+D*S*D;aa=B;ba=D;ca=R*(F*H+E*_-V)+y*(H*(-0.0-E)+F*_-Y);da=W*C;ea=Q*C;fa=C;ga=S;ha=A}A=I+0.0+$;if(A>0.0){ia=(-0.0-(L+ca*fa- +g[a+148>>2]))/A}else{ia=0.0}A=ia*+g[a+208>>2];fa=ia*+g[a+212>>2];ca=ia*+g[a+216>>2];L=ia*+g[a+220>>2];$=+(i+M*A);i=+(j+N*A);g[h>>2]=$;g[h+4>>2]=i;h=c[f>>2]|0;g[h+((c[d>>2]|0)*12|0)+8>>2]=k+K*ia*O;d=h+((c[e>>2]|0)*12|0)|0;O=+(n+da*fa);n=+(o+ea*fa);g[d>>2]=O;g[d+4>>2]=n;d=c[f>>2]|0;g[d+((c[e>>2]|0)*12|0)+8>>2]=p+ba*ia*ga;e=d+((c[l>>2]|0)*12|0)|0;ga=+(r-M*ca);M=+(s-N*ca);g[e>>2]=ga;g[e+4>>2]=M;e=c[f>>2]|0;g[e+((c[l>>2]|0)*12|0)+8>>2]=t-J*ia*P;l=e+((c[m>>2]|0)*12|0)|0;P=+(v-da*L);da=+(w-ea*L);g[l>>2]=P;g[l+4>>2]=da;g[(c[f>>2]|0)+((c[m>>2]|0)*12|0)+8>>2]=x-aa*ia*ha;return 1}function Yg(a,b){a=a|0;b=b|0;var d=0,e=0.0,f=0.0,h=0.0,i=0.0,j=0.0;d=c[b+48>>2]|0;e=+g[d+24>>2];f=+g[b+92>>2];h=+g[d+20>>2];i=+g[b+96>>2];j=f*h+e*i+ +g[d+16>>2];g[a>>2]=+g[d+12>>2]+(e*f-h*i);g[a+4>>2]=j;return}function Zg(a,b){a=a|0;b=b|0;var d=0,e=0.0,f=0.0,h=0.0,i=0.0,j=0.0;d=c[b+52>>2]|0;e=+g[d+24>>2];f=+g[b+100>>2];h=+g[d+20>>2];i=+g[b+104>>2];j=f*h+e*i+ +g[d+16>>2];g[a>>2]=+g[d+12>>2]+(e*f-h*i);g[a+4>>2]=j;return}function _g(a,b,c){a=a|0;b=b|0;c=+c;var d=0.0,e=0.0;d=+g[b+156>>2];e=d*+g[b+244>>2]*c;g[a>>2]=d*+g[b+240>>2]*c;g[a+4>>2]=e;return}function $g(a,b){a=a|0;b=+b;return+(+g[a+156>>2]*+g[a+256>>2]*b)}function ah(a){a=a|0;return+(+g[a+152>>2])}function bh(a){a=a|0;var b=0,e=0,f=0,j=0,k=0,l=0;b=i;e=c[(c[a+48>>2]|0)+8>>2]|0;f=c[(c[a+52>>2]|0)+8>>2]|0;j=c[(c[a+68>>2]|0)+56>>2]|0;k=c[(c[a+72>>2]|0)+56>>2]|0;gn(3176,(l=i,i=i+1|0,i=i+7&-8,c[l>>2]=0,l)|0);i=l;gn(3656,(l=i,i=i+8|0,c[l>>2]=e,l)|0);i=l;gn(2880,(l=i,i=i+8|0,c[l>>2]=f,l)|0);i=l;gn(2472,(l=i,i=i+8|0,c[l>>2]=d[a+61|0]|0,l)|0);i=l;gn(2032,(l=i,i=i+8|0,c[l>>2]=j,l)|0);i=l;gn(1568,(l=i,i=i+8|0,c[l>>2]=k,l)|0);i=l;gn(1184,(l=i,i=i+8|0,h[l>>3]=+g[a+152>>2],l)|0);i=l;gn(816,(l=i,i=i+8|0,c[l>>2]=c[a+56>>2],l)|0);i=l;i=b;return}function ch(a){a=a|0;return}function dh(a){a=a|0;Jn(a);return}function eh(a,b,d,e,f){a=a|0;b=b|0;d=d|0;e=e|0;f=f|0;var h=0,i=0.0,j=0,k=0.0,l=0.0,m=0,n=0.0,o=0,p=0.0,q=0.0;c[a+8>>2]=b;c[a+12>>2]=d;h=e|0;i=+g[h>>2]- +g[b+12>>2];j=e+4|0;k=+g[j>>2]- +g[b+16>>2];e=b+24|0;l=+g[e>>2];m=b+20|0;n=+g[m>>2];o=a+20|0;p=+(i*l+k*n);q=+(l*k+i*(-0.0-n));g[o>>2]=p;g[o+4>>2]=q;q=+g[h>>2]- +g[d+12>>2];p=+g[j>>2]- +g[d+16>>2];n=+g[d+24>>2];i=+g[d+20>>2];j=a+28|0;k=+(q*n+p*i);l=+(n*p+q*(-0.0-i));g[j>>2]=k;g[j+4>>2]=l;l=+g[e>>2];k=+g[f>>2];i=+g[m>>2];q=+g[f+4>>2];f=a+36|0;p=+(l*k+i*q);n=+(k*(-0.0-i)+l*q);g[f>>2]=p;g[f+4>>2]=n;g[a+44>>2]=+g[d+72>>2]- +g[b+72>>2];return}function fh(b,d){b=b|0;d=d|0;var e=0,f=0,h=0,i=0,j=0,l=0.0,m=0.0,n=0.0,o=0.0,p=0.0,q=0.0;Kh(b|0,d|0);c[b>>2]=4960;e=b+84|0;f=d+20|0;h=b+68|0;i=c[f+4>>2]|0;c[h>>2]=c[f>>2];c[h+4>>2]=i;i=d+28|0;h=b+76|0;f=c[i+4>>2]|0;c[h>>2]=c[i>>2];c[h+4>>2]=f;f=d+36|0;h=e;i=c[f>>2]|0;j=c[f+4>>2]|0;c[h>>2]=i;c[h+4>>2]=j;l=(c[k>>2]=i,+g[k>>2]);m=(c[k>>2]=j,+g[k>>2]);n=+R(l*l+m*m);if(n<1.1920928955078125e-7){o=m;p=l}else{q=1.0/n;n=l*q;g[e>>2]=n;l=m*q;g[b+88>>2]=l;o=l;p=n}e=b+92|0;n=+(o*-1.0);o=+p;g[e>>2]=n;g[e+4>>2]=o;g[b+100>>2]=+g[d+44>>2];g[b+252>>2]=0.0;Nn(b+104|0,0,16)|0;g[b+120>>2]=+g[d+52>>2];g[b+124>>2]=+g[d+56>>2];g[b+128>>2]=+g[d+64>>2];g[b+132>>2]=+g[d+68>>2];a[b+136|0]=a[d+48|0]|0;a[b+137|0]=a[d+60|0]|0;c[b+140>>2]=0;Nn(b+184|0,0,16)|0;return}function gh(b,d){b=b|0;d=d|0;var e=0,f=0,h=0,i=0,j=0,l=0,m=0,n=0,o=0,p=0,q=0,r=0,s=0.0,t=0.0,u=0.0,v=0.0,w=0.0,x=0.0,y=0.0,z=0.0,A=0.0,B=0.0,C=0.0,D=0.0,E=0.0,F=0.0,G=0.0,H=0.0,I=0.0,J=0.0,K=0.0,L=0.0,M=0.0,N=0.0,O=0.0,P=0.0,Q=0.0,R=0.0,S=0.0,V=0.0,W=0.0,X=0.0,Y=0.0,Z=0.0,_=0.0,$=0.0,aa=0.0,ba=0.0,ca=0.0,da=0,ea=0,fa=0,ga=0,ha=0,ia=0,ja=0,ka=0,la=0,ma=0,na=0,oa=0,pa=0,qa=0,ra=0,sa=0,ta=0,ua=0,va=0,wa=0,xa=0,ya=0,za=0,Aa=0,Ba=0,Ca=0,Da=0,Ea=0,Fa=0,Ga=0,Ha=0,Ia=0,Ja=0,Ka=0,La=0,Ma=0;e=c[b+48>>2]|0;f=c[e+8>>2]|0;h=b+144|0;c[h>>2]=f;i=c[b+52>>2]|0;j=c[i+8>>2]|0;l=b+148|0;c[l>>2]=j;m=e+44|0;n=b+152|0;o=c[m>>2]|0;p=c[m+4>>2]|0;c[n>>2]=o;c[n+4>>2]=p;n=i+44|0;m=b+160|0;q=c[n>>2]|0;r=c[n+4>>2]|0;c[m>>2]=q;c[m+4>>2]=r;s=+g[e+136>>2];g[b+168>>2]=s;t=+g[i+136>>2];g[b+172>>2]=t;u=+g[e+144>>2];g[b+176>>2]=u;v=+g[i+144>>2];g[b+180>>2]=v;i=c[d+28>>2]|0;e=i+(f*12|0)|0;w=+g[e>>2];x=+g[e+4>>2];y=+g[i+(f*12|0)+8>>2];e=d+32|0;m=c[e>>2]|0;n=m+(f*12|0)|0;z=+g[n>>2];A=+g[n+4>>2];B=+g[m+(f*12|0)+8>>2];n=i+(j*12|0)|0;C=+g[n>>2];D=+g[n+4>>2];E=+g[i+(j*12|0)+8>>2];i=m+(j*12|0)|0;F=+g[i>>2];G=+g[i+4>>2];H=+g[m+(j*12|0)+8>>2];I=+U(y);J=+T(y);y=+U(E);K=+T(E);E=+g[b+68>>2]-(c[k>>2]=o,+g[k>>2]);L=+g[b+72>>2]-(c[k>>2]=p,+g[k>>2]);M=J*E-I*L;N=I*E+J*L;L=+g[b+76>>2]-(c[k>>2]=q,+g[k>>2]);E=+g[b+80>>2]-(c[k>>2]=r,+g[k>>2]);O=K*L-y*E;P=y*L+K*E;E=C-w+O-M;w=D-x+P-N;x=+g[b+84>>2];D=+g[b+88>>2];C=J*x-I*D;K=I*x+J*D;r=b+184|0;D=+C;x=+K;g[r>>2]=D;g[r+4>>2]=x;x=M+E;M=N+w;N=x*K-M*C;g[b+208>>2]=N;D=O*K-P*C;g[b+212>>2]=D;L=s+t;y=u*N;Q=v*D;R=L+N*y+D*Q;if(R>0.0){S=1.0/R}else{S=R}g[b+252>>2]=S;S=+g[b+92>>2];V=+g[b+96>>2];W=J*S-I*V;X=I*S+J*V;r=b+192|0;V=+W;J=+X;g[r>>2]=V;g[r+4>>2]=J;J=x*X-M*W;g[b+200>>2]=J;M=O*X-P*W;g[b+204>>2]=M;P=u*J;O=v*M;x=P+O;V=P*N+O*D;S=v+u;I=y+Q;g[b+216>>2]=L+J*P+M*O;g[b+220>>2]=x;g[b+224>>2]=V;g[b+228>>2]=x;g[b+232>>2]=S==0.0?1.0:S;g[b+236>>2]=I;g[b+240>>2]=V;g[b+244>>2]=I;g[b+248>>2]=R;do{if((a[b+136|0]|0)==0){c[b+140>>2]=0;g[b+112>>2]=0.0}else{R=E*C+w*K;I=+g[b+124>>2];V=+g[b+120>>2];S=I-V;if(S>0.0){Y=S}else{Y=-0.0-S}if(Y<.009999999776482582){c[b+140>>2]=3;break}if(!(R>V)){r=b+140|0;if((c[r>>2]|0)==1){break}c[r>>2]=1;g[b+112>>2]=0.0;break}r=b+140|0;if(R<I){c[r>>2]=0;g[b+112>>2]=0.0;break}if((c[r>>2]|0)==2){break}c[r>>2]=2;g[b+112>>2]=0.0}}while(0);if((a[b+137|0]|0)==0){g[b+116>>2]=0.0}r=b+104|0;if((a[d+24|0]|0)==0){Nn(r|0,0,16)|0;Z=B;_=H;$=F;aa=G;ba=z;ca=A;da=c[e>>2]|0;ea=da+(f*12|0)|0;fa=ea;ga=(g[k>>2]=ba,c[k>>2]|0);ha=(g[k>>2]=ca,c[k>>2]|0);ia=ha;ja=0;ka=0;la=ia;ma=ga;na=0;oa=ka|ma;pa=la|na;qa=fa|0;c[qa>>2]=oa;ra=fa+4|0;c[ra>>2]=pa;sa=c[h>>2]|0;ta=c[e>>2]|0;ua=ta+(sa*12|0)+8|0;g[ua>>2]=Z;va=c[l>>2]|0;wa=ta+(va*12|0)|0;xa=wa;ya=(g[k>>2]=$,c[k>>2]|0);za=(g[k>>2]=aa,c[k>>2]|0);Aa=za;Ba=0;Ca=0;Da=Aa;Ea=ya;Fa=0;Ga=Ca|Ea;Ha=Da|Fa;Ia=xa|0;c[Ia>>2]=Ga;Ja=xa+4|0;c[Ja>>2]=Ha;Ka=c[l>>2]|0;La=c[e>>2]|0;Ma=La+(Ka*12|0)+8|0;g[Ma>>2]=_;return}else{q=d+8|0;Y=+g[q>>2];d=r|0;w=Y*+g[d>>2];g[d>>2]=w;d=b+108|0;E=Y*+g[d>>2];g[d>>2]=E;d=b+112|0;I=Y*+g[d>>2];g[d>>2]=I;d=b+116|0;Y=+g[q>>2]*+g[d>>2];g[d>>2]=Y;R=Y+I;I=w*W+C*R;C=w*X+R*K;Z=B-u*(w*J+E+R*N);_=H+v*(E+w*M+R*D);$=F+t*I;aa=G+t*C;ba=z-s*I;ca=A-s*C;da=c[e>>2]|0;ea=da+(f*12|0)|0;fa=ea;ga=(g[k>>2]=ba,c[k>>2]|0);ha=(g[k>>2]=ca,c[k>>2]|0);ia=ha;ja=0;ka=0;la=ia;ma=ga;na=0;oa=ka|ma;pa=la|na;qa=fa|0;c[qa>>2]=oa;ra=fa+4|0;c[ra>>2]=pa;sa=c[h>>2]|0;ta=c[e>>2]|0;ua=ta+(sa*12|0)+8|0;g[ua>>2]=Z;va=c[l>>2]|0;wa=ta+(va*12|0)|0;xa=wa;ya=(g[k>>2]=$,c[k>>2]|0);za=(g[k>>2]=aa,c[k>>2]|0);Aa=za;Ba=0;Ca=0;Da=Aa;Ea=ya;Fa=0;Ga=Ca|Ea;Ha=Da|Fa;Ia=xa|0;c[Ia>>2]=Ga;Ja=xa+4|0;c[Ja>>2]=Ha;Ka=c[l>>2]|0;La=c[e>>2]|0;Ma=La+(Ka*12|0)+8|0;g[Ma>>2]=_;return}}function hh(b,d){b=b|0;d=d|0;var e=0,f=0,h=0,j=0,k=0,l=0,m=0,n=0,o=0,p=0,q=0,r=0,s=0.0,t=0.0,u=0.0,v=0,w=0.0,x=0.0,y=0.0,z=0.0,A=0.0,B=0.0,C=0.0,D=0.0,E=0.0,F=0.0,G=0.0,H=0.0,I=0.0,J=0.0,K=0.0,L=0.0,M=0.0,N=0.0,O=0.0,P=0.0,Q=0.0,R=0.0,S=0,T=0,U=0,V=0,W=0,X=0,Y=0,Z=0,_=0,$=0,aa=0,ba=0,ca=0,da=0,ea=0.0,fa=0.0,ga=0.0,ha=0.0,ia=0.0;e=i;i=i+64|0;f=e|0;h=e+16|0;j=e+32|0;k=e+40|0;l=e+48|0;m=e+56|0;n=b+144|0;o=c[n>>2]|0;p=d+32|0;q=c[p>>2]|0;r=q+(o*12|0)|0;s=+g[r>>2];t=+g[r+4>>2];u=+g[q+(o*12|0)+8>>2];o=b+148|0;r=c[o>>2]|0;v=q+(r*12|0)|0;w=+g[v>>2];x=+g[v+4>>2];y=+g[q+(r*12|0)+8>>2];z=+g[b+168>>2];A=+g[b+172>>2];B=+g[b+176>>2];C=+g[b+180>>2];do{if((a[b+137|0]|0)==0){D=u;E=y;F=w;G=x;H=s;I=t}else{if((c[b+140>>2]|0)==3){D=u;E=y;F=w;G=x;H=s;I=t;break}J=+g[b+184>>2];K=+g[b+188>>2];L=+g[b+212>>2];M=+g[b+208>>2];r=b+116|0;N=+g[r>>2];O=+g[d>>2]*+g[b+128>>2];P=N+ +g[b+252>>2]*(+g[b+132>>2]-((w-s)*J+(x-t)*K+y*L-u*M));Q=-0.0-O;R=P<O?P:O;O=R<Q?Q:R;g[r>>2]=O;R=O-N;N=J*R;J=K*R;D=u-B*M*R;E=y+C*L*R;F=w+A*N;G=x+A*J;H=s-z*N;I=t-z*J}}while(0);t=F-H;s=G-I;d=b+192|0;r=b+196|0;q=b+204|0;v=b+200|0;x=t*+g[d>>2]+s*+g[r>>2]+E*+g[q>>2]-D*+g[v>>2];w=E-D;do{if((a[b+136|0]|0)==0){S=10}else{T=b+140|0;if((c[T>>2]|0)==0){S=10;break}U=b+184|0;V=b+188|0;W=b+212|0;X=b+208|0;Y=b+104|0;y=+g[Y>>2];Z=b+108|0;u=+g[Z>>2];_=b+112|0;J=+g[_>>2];$=b+216|0;N=-0.0-x;R=-0.0-w;L=-0.0-(t*+g[U>>2]+s*+g[V>>2]+E*+g[W>>2]-D*+g[X>>2]);g[h>>2]=N;g[h+4>>2]=R;g[h+8>>2]=L;Xm(f,$,h);aa=f|0;g[Y>>2]=+g[aa>>2]+ +g[Y>>2];ba=f+4|0;g[Z>>2]=+g[ba>>2]+ +g[Z>>2];ca=f+8|0;L=+g[ca>>2]+ +g[_>>2];g[_>>2]=L;da=c[T>>2]|0;if((da|0)==1){M=L>0.0?L:0.0;g[_>>2]=M;ea=M}else if((da|0)==2){M=L<0.0?L:0.0;g[_>>2]=M;ea=M}else{ea=L}L=ea-J;M=R-L*+g[b+244>>2];g[j>>2]=N- +g[b+240>>2]*L;g[j+4>>2]=M;Ym(k,$,j);M=y+ +g[k>>2];L=u+ +g[k+4>>2];g[Y>>2]=M;g[Z>>2]=L;N=M-y;y=L-u;u=+g[_>>2]-J;g[aa>>2]=N;g[ba>>2]=y;g[ca>>2]=u;fa=N*+g[v>>2]+y+u*+g[X>>2];ga=y+N*+g[q>>2]+u*+g[W>>2];ha=N*+g[r>>2]+u*+g[V>>2];ia=N*+g[d>>2]+u*+g[U>>2]}}while(0);if((S|0)==10){g[m>>2]=-0.0-x;g[m+4>>2]=-0.0-w;Ym(l,b+216|0,m);w=+g[l>>2];m=b+104|0;g[m>>2]=w+ +g[m>>2];x=+g[l+4>>2];l=b+108|0;g[l>>2]=x+ +g[l>>2];fa=w*+g[v>>2]+x;ga=x+w*+g[q>>2];ha=w*+g[r>>2];ia=w*+g[d>>2]}d=(c[p>>2]|0)+((c[n>>2]|0)*12|0)|0;w=+(H-z*ia);H=+(I-z*ha);g[d>>2]=w;g[d+4>>2]=H;d=c[p>>2]|0;g[d+((c[n>>2]|0)*12|0)+8>>2]=D-B*fa;n=d+((c[o>>2]|0)*12|0)|0;fa=+(F+A*ia);ia=+(G+A*ha);g[n>>2]=fa;g[n+4>>2]=ia;g[(c[p>>2]|0)+((c[o>>2]|0)*12|0)+8>>2]=E+C*ga;i=e;return}function ih(b,d){b=b|0;d=d|0;var e=0,f=0,h=0,j=0,k=0,l=0,m=0,n=0,o=0.0,p=0.0,q=0.0,r=0,s=0,t=0.0,u=0.0,v=0.0,w=0.0,x=0.0,y=0.0,z=0.0,A=0.0,B=0.0,C=0.0,D=0.0,E=0.0,F=0.0,G=0.0,H=0.0,I=0.0,J=0.0,K=0.0,L=0.0,M=0.0,N=0.0,O=0.0,P=0.0,Q=0.0,R=0.0,S=0.0,V=0,W=0.0,X=0.0,Y=0.0,Z=0.0,_=0.0,$=0.0,aa=0.0,ba=0,ca=0,da=0.0,ea=0;e=i;i=i+72|0;f=e|0;h=e+40|0;j=e+56|0;k=b+144|0;l=c[k>>2]|0;m=d+28|0;d=c[m>>2]|0;n=d+(l*12|0)|0;o=+g[n>>2];p=+g[n+4>>2];q=+g[d+(l*12|0)+8>>2];n=b+148|0;r=c[n>>2]|0;s=d+(r*12|0)|0;t=+g[s>>2];u=+g[s+4>>2];v=+g[d+(r*12|0)+8>>2];w=+U(q);x=+T(q);y=+U(v);z=+T(v);A=+g[b+168>>2];B=+g[b+172>>2];C=+g[b+176>>2];D=+g[b+180>>2];E=+g[b+68>>2]- +g[b+152>>2];F=+g[b+72>>2]- +g[b+156>>2];G=x*E-w*F;H=w*E+x*F;F=+g[b+76>>2]- +g[b+160>>2];E=+g[b+80>>2]- +g[b+164>>2];I=z*F-y*E;J=y*F+z*E;E=t+I-o-G;z=u+J-p-H;F=+g[b+84>>2];y=+g[b+88>>2];K=x*F-w*y;L=w*F+x*y;y=G+E;G=H+z;H=L*y-K*G;F=I*L-J*K;M=+g[b+92>>2];N=+g[b+96>>2];O=x*M-w*N;P=w*M+x*N;N=P*y-O*G;G=I*P-J*O;J=O*E+P*z;I=v-q- +g[b+100>>2];if(J>0.0){Q=J}else{Q=-0.0-J}if(I>0.0){R=I}else{R=-0.0-I}do{if((a[b+136|0]|0)==0){S=Q;V=0;W=0.0}else{y=K*E+L*z;x=+g[b+124>>2];M=+g[b+120>>2];w=x-M;if(w>0.0){X=w}else{X=-0.0-w}if(X<.009999999776482582){w=y<.20000000298023224?y:.20000000298023224;if(y>0.0){Y=y}else{Y=-0.0-y}S=Q>Y?Q:Y;V=1;W=w<-.20000000298023224?-.20000000298023224:w;break}if(!(y>M)){w=y-M+.004999999888241291;Z=w<0.0?w:0.0;w=M-y;S=Q>w?Q:w;V=1;W=Z<-.20000000298023224?-.20000000298023224:Z;break}if(y<x){S=Q;V=0;W=0.0;break}Z=y-x;x=Z+-.004999999888241291;y=x<.20000000298023224?x:.20000000298023224;S=Q>Z?Q:Z;V=1;W=y<0.0?0.0:y}}while(0);Q=A+B;Y=C*N;X=D*G;z=G*X+(Q+N*Y);E=X+Y;if(V){y=F*X+H*Y;Y=C+D;X=C*H;Z=D*F;x=Z+X;g[f>>2]=z;g[f+4>>2]=E;g[f+8>>2]=y;g[f+12>>2]=E;g[f+16>>2]=Y==0.0?1.0:Y;g[f+20>>2]=x;g[f+24>>2]=y;g[f+28>>2]=x;g[f+32>>2]=F*Z+(Q+H*X);g[j>>2]=-0.0-J;g[j+4>>2]=-0.0-I;g[j+8>>2]=-0.0-W;Xm(h,f,j);_=+g[h>>2];$=+g[h+4>>2];aa=+g[h+8>>2];ba=c[k>>2]|0;ca=c[m>>2]|0}else{W=C+D;X=W==0.0?1.0:W;W=-0.0-J;J=-0.0-I;I=X*z-E*E;if(I!=0.0){da=1.0/I}else{da=I}_=(X*W-E*J)*da;$=(z*J-E*W)*da;aa=0.0;ba=l;ca=d}da=K*aa+O*_;O=L*aa+P*_;d=ca+(ba*12|0)|0;P=+(o-A*da);o=+(p-A*O);g[d>>2]=P;g[d+4>>2]=o;d=c[m>>2]|0;g[d+((c[k>>2]|0)*12|0)+8>>2]=q-C*(H*aa+($+N*_));k=d+((c[n>>2]|0)*12|0)|0;N=+(t+B*da);da=+(u+B*O);g[k>>2]=N;g[k+4>>2]=da;g[(c[m>>2]|0)+((c[n>>2]|0)*12|0)+8>>2]=v+D*(F*aa+($+G*_));if(S>.004999999888241291){ea=0;i=e;return ea|0}ea=R<=.03490658849477768;i=e;return ea|0}function jh(a,b){a=a|0;b=b|0;var d=0,e=0.0,f=0.0,h=0.0,i=0.0,j=0.0;d=c[b+48>>2]|0;e=+g[d+24>>2];f=+g[b+68>>2];h=+g[d+20>>2];i=+g[b+72>>2];j=f*h+e*i+ +g[d+16>>2];g[a>>2]=+g[d+12>>2]+(e*f-h*i);g[a+4>>2]=j;return}function kh(a,b){a=a|0;b=b|0;var d=0,e=0.0,f=0.0,h=0.0,i=0.0,j=0.0;d=c[b+52>>2]|0;e=+g[d+24>>2];f=+g[b+76>>2];h=+g[d+20>>2];i=+g[b+80>>2];j=f*h+e*i+ +g[d+16>>2];g[a>>2]=+g[d+12>>2]+(e*f-h*i);g[a+4>>2]=j;return}function lh(a,b,c){a=a|0;b=b|0;c=+c;var d=0.0,e=0.0,f=0.0;d=+g[b+104>>2];e=+g[b+116>>2]+ +g[b+112>>2];f=(d*+g[b+196>>2]+e*+g[b+188>>2])*c;g[a>>2]=(d*+g[b+192>>2]+ +g[b+184>>2]*e)*c;g[a+4>>2]=f;return}function mh(a,b){a=a|0;b=+b;return+(+g[a+108>>2]*b)}function nh(a){a=a|0;var b=0,d=0.0,e=0.0,f=0.0,h=0.0,i=0,j=0.0,k=0.0,l=0.0,m=0.0,n=0.0,o=0.0;b=c[a+48>>2]|0;d=+g[b+24>>2];e=+g[a+68>>2];f=+g[b+20>>2];h=+g[a+72>>2];i=c[a+52>>2]|0;j=+g[i+24>>2];k=+g[a+76>>2];l=+g[i+20>>2];m=+g[a+80>>2];n=+g[a+84>>2];o=+g[a+88>>2];return+((+g[i+12>>2]+(j*k-l*m)-(+g[b+12>>2]+(d*e-f*h)))*(d*n-f*o)+(k*l+j*m+ +g[i+16>>2]-(e*f+d*h+ +g[b+16>>2]))*(f*n+d*o))}function oh(b){b=b|0;return(a[b+136|0]|0)!=0|0}function ph(e,f){e=e|0;f=f|0;var h=0,i=0,j=0,k=0;h=e+136|0;if((f&1|0)==(d[h]|0|0)){return}i=c[e+48>>2]|0;j=i+4|0;k=b[j>>1]|0;if((k&2)==0){b[j>>1]=k|2;g[i+160>>2]=0.0}i=c[e+52>>2]|0;k=i+4|0;j=b[k>>1]|0;if((j&2)==0){b[k>>1]=j|2;g[i+160>>2]=0.0}a[h]=f&1;g[e+112>>2]=0.0;return}function qh(b){b=b|0;return(a[b+137|0]|0)!=0|0}function rh(d,e){d=d|0;e=e|0;var f=0,h=0,i=0,j=0,k=0;f=c[d+48>>2]|0;h=f+4|0;i=b[h>>1]|0;if((i&2)==0){b[h>>1]=i|2;g[f+160>>2]=0.0}f=c[d+52>>2]|0;i=f+4|0;h=b[i>>1]|0;if(!((h&2)==0)){j=d+137|0;k=e&1;a[j]=k;return}b[i>>1]=h|2;g[f+160>>2]=0.0;j=d+137|0;k=e&1;a[j]=k;return}function sh(a,d){a=a|0;d=+d;var e=0,f=0,h=0,i=0;e=c[a+48>>2]|0;f=e+4|0;h=b[f>>1]|0;if((h&2)==0){b[f>>1]=h|2;g[e+160>>2]=0.0}e=c[a+52>>2]|0;h=e+4|0;f=b[h>>1]|0;if(!((f&2)==0)){i=a+132|0;g[i>>2]=d;return}b[h>>1]=f|2;g[e+160>>2]=0.0;i=a+132|0;g[i>>2]=d;return}function th(a,b){a=a|0;b=+b;return+(+g[a+116>>2]*b)}function uh(a){a=a|0;var b=0,e=0,f=0,j=0,k=0.0;b=i;e=c[(c[a+48>>2]|0)+8>>2]|0;f=c[(c[a+52>>2]|0)+8>>2]|0;gn(3144,(j=i,i=i+1|0,i=i+7&-8,c[j>>2]=0,j)|0);i=j;gn(3624,(j=i,i=i+8|0,c[j>>2]=e,j)|0);i=j;gn(2848,(j=i,i=i+8|0,c[j>>2]=f,j)|0);i=j;gn(2432,(j=i,i=i+8|0,c[j>>2]=d[a+61|0]|0,j)|0);i=j;k=+g[a+72>>2];gn(1984,(j=i,i=i+16|0,h[j>>3]=+g[a+68>>2],h[j+8>>3]=k,j)|0);i=j;k=+g[a+80>>2];gn(1520,(j=i,i=i+16|0,h[j>>3]=+g[a+76>>2],h[j+8>>3]=k,j)|0);i=j;k=+g[a+88>>2];gn(1144,(j=i,i=i+16|0,h[j>>3]=+g[a+84>>2],h[j+8>>3]=k,j)|0);i=j;gn(784,(j=i,i=i+8|0,h[j>>3]=+g[a+100>>2],j)|0);i=j;gn(440,(j=i,i=i+8|0,c[j>>2]=d[a+136|0]|0,j)|0);i=j;gn(160,(j=i,i=i+8|0,h[j>>3]=+g[a+120>>2],j)|0);i=j;gn(4072,(j=i,i=i+8|0,h[j>>3]=+g[a+124>>2],j)|0);i=j;gn(3928,(j=i,i=i+8|0,c[j>>2]=d[a+137|0]|0,j)|0);i=j;gn(3528,(j=i,i=i+8|0,h[j>>3]=+g[a+132>>2],j)|0);i=j;gn(3320,(j=i,i=i+8|0,h[j>>3]=+g[a+128>>2],j)|0);i=j;gn(3200,(j=i,i=i+8|0,c[j>>2]=c[a+56>>2],j)|0);i=j;i=b;return}function vh(a){a=a|0;return}function wh(a){a=a|0;Jn(a);return}function xh(a,b){a=a|0;b=b|0;var d=0,e=0,f=0;Kh(a|0,b|0);c[a>>2]=5848;d=b+20|0;e=a+68|0;f=c[d+4>>2]|0;c[e>>2]=c[d>>2];c[e+4>>2]=f;f=b+28|0;e=a+76|0;d=c[f+4>>2]|0;c[e>>2]=c[f>>2];c[e+4>>2]=d;g[a+84>>2]=+g[b+36>>2];g[a+160>>2]=0.0;g[a+92>>2]=0.0;c[a+164>>2]=0;g[a+88>>2]=0.0;return}



function xb(a){a=a|0;var b=0;b=i;i=i+a|0;i=i+7&-8;return b|0}function yb(){return i|0}function zb(a){a=a|0;i=a}function Ab(a,b){a=a|0;b=b|0;if((s|0)==0){s=a;t=b}}function Bb(b){b=b|0;a[k]=a[b];a[k+1|0]=a[b+1|0];a[k+2|0]=a[b+2|0];a[k+3|0]=a[b+3|0]}function Cb(b){b=b|0;a[k]=a[b];a[k+1|0]=a[b+1|0];a[k+2|0]=a[b+2|0];a[k+3|0]=a[b+3|0];a[k+4|0]=a[b+4|0];a[k+5|0]=a[b+5|0];a[k+6|0]=a[b+6|0];a[k+7|0]=a[b+7|0]}function Db(a){a=a|0;F=a}function Eb(a){a=a|0;G=a}function Fb(a){a=a|0;H=a}function Gb(a){a=a|0;I=a}function Hb(a){a=a|0;J=a}function Ib(a){a=a|0;K=a}function Jb(a){a=a|0;L=a}function Kb(a){a=a|0;M=a}function Lb(a){a=a|0;N=a}function Mb(a){a=a|0;O=a}function Nb(){c[1924]=o+8;c[1928]=o+8;c[1932]=o+8;c[1936]=o+8;c[1940]=o+8;c[1944]=o+8;c[1948]=o+8;c[1952]=p+8;c[1954]=p+8;c[1956]=o+8;c[1960]=p+8;c[1962]=p+8;c[1964]=o+8;c[1968]=o+8;c[1972]=o+8;c[1976]=p+8;c[1978]=p+8;c[1980]=p+8;c[1982]=o+8;c[1986]=o+8;c[1990]=o+8;c[1994]=o+8;c[1998]=o+8;c[2002]=o+8;c[2006]=o+8;c[2010]=p+8;c[2012]=p+8;c[2014]=o+8;c[2018]=o+8;c[2022]=o+8;c[2026]=o+8;c[2030]=p+8;c[2032]=o+8;c[2036]=o+8;c[2040]=p+8;c[2042]=o+8;c[2046]=o+8;c[2050]=o+8;c[2054]=o+8;c[2058]=o+8;c[2062]=o+8;c[2066]=o+8;c[2070]=o+8;c[2074]=o+8;c[2078]=o+8;c[2082]=o+8;c[2086]=o+8;c[2090]=o+8}function Ob(a){a=a|0;return+(+(c[a+60>>2]|0))}function Pb(d,e,f,j,k,l,m,n,o,p,q){d=d|0;e=+e;f=+f;j=+j;k=+k;l=+l;m=+m;n=+n;o=+o;p=p|0;q=+q;var r=0,s=0,t=0,u=0,v=0,w=0,x=0,y=0,z=0,A=0;r=i;i=i+1104|0;s=r|0;t=r+8|0;u=r+40|0;v=r+80|0;h[s>>3]=l;w=t|0;c[w>>2]=0;g[t+16>>2]=e;g[t+8>>2]=f;a[t+20|0]=j!=0.0|0;g[t+12>>2]=k;c[t+4>>2]=s;b[t+22>>1]=~~m;b[t+26>>1]=~~n;b[t+24>>1]=~~o;c[u>>2]=5728;c[u+4>>2]=3;g[u+8>>2]=.009999999776482582;c[u+12>>2]=0;c[u+16>>2]=0;a[u+36|0]=0;a[u+37|0]=0;s=v|0;x=~~(q*.5);if(q>0.0){y=0;z=0;while(1){A=v+(y<<3)|0;o=+(+g[p+(z<<2)>>2]);n=+(+g[p+((z|1)<<2)>>2]);g[A>>2]=o;g[A+4>>2]=n;A=z+2|0;if(+(A|0)<q){y=y+1|0;z=A}else{break}}}Se(u,s,x);c[w>>2]=u;w=$j(d,t)|0;Re(u);i=r;return w|0}function Qb(d,e,f,j,k,l,m,n,o,p,q,r){d=d|0;e=+e;f=+f;j=+j;k=+k;l=+l;m=+m;n=+n;o=+o;p=+p;q=+q;r=+r;var s=0,t=0,u=0,v=0;s=i;i=i+64|0;t=s|0;u=s+8|0;v=s+40|0;h[t>>3]=l;g[u+16>>2]=e;g[u+8>>2]=f;a[u+20|0]=j!=0.0|0;g[u+12>>2]=k;c[u+4>>2]=t;b[u+22>>1]=~~m;b[u+26>>1]=~~n;b[u+24>>1]=~~o;c[v>>2]=5480;Nn(v+4|0,0,12)|0;g[v+12>>2]=p;g[v+16>>2]=q;g[v+8>>2]=r;c[u>>2]=v;v=$j(d,u)|0;i=s;return v|0}function Rb(a){a=a|0;return}function Sb(b,d,e,f,j,k,l,m,n,o,p,q,r,s,t,u,v,w,x,y,z,A,B){b=b|0;d=+d;e=+e;f=+f;j=+j;k=+k;l=+l;m=+m;n=+n;o=+o;p=+p;q=+q;r=+r;s=+s;t=+t;u=+u;v=+v;w=+w;x=+x;y=+y;z=+z;A=+A;B=+B;var C=0,D=0,E=0,F=0,G=0,H=0,I=0;C=i;i=i+112|0;D=C|0;E=C+8|0;F=C+88|0;h[D>>3]=y;G=E+8|0;H=E+20|0;I=E+40|0;Nn(I|0,0,28)|0;g[E+16>>2]=d;g[E+28>>2]=e;a[E+32|0]=~~f;a[E+33|0]=~~j;a[E+34|0]=~~k;a[E+35|0]=~~l;c[E>>2]=~~m;c[E+72>>2]=0;c[E+4>>2]=~~o;g[E+64>>2]=p;p=+q;q=+r;g[H>>2]=p;g[H+4>>2]=q;q=+s;s=+t;g[G>>2]=q;g[G+4>>2]=s;c[E+60>>2]=0;c[E+56>>2]=~~v;c[E+48>>2]=0;c[E+44>>2]=0;g[E+36>>2]=w;g[E+52>>2]=x;c[E+68>>2]=D;c[F>>2]=5480;c[F+4>>2]=0;D=F+12|0;x=+z;z=+A;g[D>>2]=x;g[D+4>>2]=z;g[F+8>>2]=B;c[I>>2]=F;F=Uk(b,E)|0;i=C;return F|0}function Tb(a,b,d,e,f,h,j,k){a=a|0;b=+b;d=+d;e=+e;f=+f;h=+h;j=+j;k=+k;var l=0,m=0,n=0;l=i;i=i+40|0;m=l|0;n=l+24|0;c[m>>2]=5480;Nn(m+4|0,0,12)|0;g[m+12>>2]=b;g[m+16>>2]=d;g[m+8>>2]=e;g[n>>2]=f;g[n+4>>2]=h;g[n+8>>2]=j;g[n+12>>2]=k;k=+(Rk(a,m|0,n,0)|0);i=l;return+k}function Ub(d,e,f,j,k,l,m,n,o,p,q,r,s,t,u,v,w,x,y){d=d|0;e=+e;f=+f;j=+j;k=+k;l=+l;m=+m;n=+n;o=+o;p=+p;q=+q;r=+r;s=+s;t=+t;u=+u;v=+v;w=+w;x=+x;y=+y;var z=0,A=0,B=0,C=0;z=i;i=i+88|0;A=z|0;B=z+8|0;C=z+40|0;h[A>>3]=l;g[B+16>>2]=e;g[B+8>>2]=f;a[B+20|0]=j!=0.0|0;g[B+12>>2]=k;c[B+4>>2]=A;b[B+22>>1]=~~m;b[B+26>>1]=~~n;b[B+24>>1]=~~o;c[C>>2]=5976;c[C+4>>2]=1;g[C+8>>2]=.009999999776482582;Nn(C+28|0,0,18)|0;A=C+28|0;o=+r;r=+s;g[A>>2]=o;g[A+4>>2]=r;A=C+12|0;r=+t;t=+u;g[A>>2]=r;g[A+4>>2]=t;A=C+20|0;t=+v;v=+w;g[A>>2]=t;g[A+4>>2]=v;A=C+36|0;v=+x;x=+y;g[A>>2]=v;g[A+4>>2]=x;a[C+44|0]=p!=0.0|0;a[C+45|0]=q!=0.0|0;c[B>>2]=C;C=$j(d,B)|0;i=z;return C|0}function Vb(a){a=a|0;return}function Wb(d,e,f,j,k,l,m,n,o,p,q,r,s,t,u){d=d|0;e=+e;f=+f;j=+j;k=+k;l=+l;m=+m;n=+n;o=+o;p=+p;q=+q;r=+r;s=+s;t=+t;u=+u;var v=0,w=0,x=0,y=0,z=0;v=i;i=i+216|0;w=v|0;x=v+8|0;y=v+40|0;z=v+64|0;h[w>>3]=l;g[x+16>>2]=e;g[x+8>>2]=f;a[x+20|0]=j!=0.0|0;g[x+12>>2]=k;c[x+4>>2]=w;b[x+22>>1]=~~m;b[x+26>>1]=~~n;b[x+24>>1]=~~o;g[y>>2]=p;g[y+4>>2]=q;g[y+8>>2]=r;g[y+12>>2]=s;g[y+16>>2]=t;g[y+20>>2]=u;c[z>>2]=5360;c[z+4>>2]=2;g[z+8>>2]=.009999999776482582;c[z+148>>2]=0;g[z+12>>2]=0.0;g[z+16>>2]=0.0;De(z,y|0,3);c[x>>2]=z;z=$j(d,x)|0;i=v;return z|0}function Xb(a){a=a|0;return}function Yb(d,e,f,j,k,l,m,n,o,p,q,r,s,t,u,v,w){d=d|0;e=+e;f=+f;j=+j;k=+k;l=+l;m=+m;n=+n;o=+o;p=+p;q=+q;r=+r;s=+s;t=+t;u=+u;v=+v;w=+w;var x=0,y=0,z=0,A=0,B=0;x=i;i=i+224|0;y=x|0;z=x+8|0;A=x+40|0;B=x+72|0;h[y>>3]=l;g[z+16>>2]=e;g[z+8>>2]=f;a[z+20|0]=j!=0.0|0;g[z+12>>2]=k;c[z+4>>2]=y;b[z+22>>1]=~~m;b[z+26>>1]=~~n;b[z+24>>1]=~~o;g[A>>2]=p;g[A+4>>2]=q;g[A+8>>2]=r;g[A+12>>2]=s;g[A+16>>2]=t;g[A+20>>2]=u;g[A+24>>2]=v;g[A+28>>2]=w;c[B>>2]=5360;c[B+4>>2]=2;g[B+8>>2]=.009999999776482582;c[B+148>>2]=0;g[B+12>>2]=0.0;g[B+16>>2]=0.0;De(B,A|0,4);c[z>>2]=B;B=$j(d,z)|0;i=x;return B|0}function Zb(d,e,f,j,k,l,m,n,o,p,q,r,s,t,u,v,w,x,y){d=d|0;e=+e;f=+f;j=+j;k=+k;l=+l;m=+m;n=+n;o=+o;p=+p;q=+q;r=+r;s=+s;t=+t;u=+u;v=+v;w=+w;x=+x;y=+y;var z=0,A=0,B=0,C=0,D=0;z=i;i=i+232|0;A=z|0;B=z+8|0;C=z+40|0;D=z+80|0;h[A>>3]=l;g[B+16>>2]=e;g[B+8>>2]=f;a[B+20|0]=j!=0.0|0;g[B+12>>2]=k;c[B+4>>2]=A;b[B+22>>1]=~~m;b[B+26>>1]=~~n;b[B+24>>1]=~~o;g[C>>2]=p;g[C+4>>2]=q;g[C+8>>2]=r;g[C+12>>2]=s;g[C+16>>2]=t;g[C+20>>2]=u;g[C+24>>2]=v;g[C+28>>2]=w;g[C+32>>2]=x;g[C+36>>2]=y;c[D>>2]=5360;c[D+4>>2]=2;g[D+8>>2]=.009999999776482582;c[D+148>>2]=0;g[D+12>>2]=0.0;g[D+16>>2]=0.0;De(D,C|0,5);c[B>>2]=D;D=$j(d,B)|0;i=z;return D|0}function _b(d,e,f,j,k,l,m,n,o,p,q,r,s,t,u,v,w,x,y,z,A){d=d|0;e=+e;f=+f;j=+j;k=+k;l=+l;m=+m;n=+n;o=+o;p=+p;q=+q;r=+r;s=+s;t=+t;u=+u;v=+v;w=+w;x=+x;y=+y;z=+z;A=+A;var B=0,C=0,D=0,E=0,F=0;B=i;i=i+240|0;C=B|0;D=B+8|0;E=B+40|0;F=B+88|0;h[C>>3]=l;g[D+16>>2]=e;g[D+8>>2]=f;a[D+20|0]=j!=0.0|0;g[D+12>>2]=k;c[D+4>>2]=C;b[D+22>>1]=~~m;b[D+26>>1]=~~n;b[D+24>>1]=~~o;g[E>>2]=p;g[E+4>>2]=q;g[E+8>>2]=r;g[E+12>>2]=s;g[E+16>>2]=t;g[E+20>>2]=u;g[E+24>>2]=v;g[E+28>>2]=w;g[E+32>>2]=x;g[E+36>>2]=y;g[E+40>>2]=z;g[E+44>>2]=A;c[F>>2]=5360;c[F+4>>2]=2;g[F+8>>2]=.009999999776482582;c[F+148>>2]=0;g[F+12>>2]=0.0;g[F+16>>2]=0.0;De(F,E|0,6);c[D>>2]=F;F=$j(d,D)|0;i=B;return F|0}function $b(d,e,f,j,k,l,m,n,o,p,q,r,s,t,u,v,w,x,y,z,A,B,C){d=d|0;e=+e;f=+f;j=+j;k=+k;l=+l;m=+m;n=+n;o=+o;p=+p;q=+q;r=+r;s=+s;t=+t;u=+u;v=+v;w=+w;x=+x;y=+y;z=+z;A=+A;B=+B;C=+C;var D=0,E=0,F=0,G=0,H=0;D=i;i=i+248|0;E=D|0;F=D+8|0;G=D+40|0;H=D+96|0;h[E>>3]=l;g[F+16>>2]=e;g[F+8>>2]=f;a[F+20|0]=j!=0.0|0;g[F+12>>2]=k;c[F+4>>2]=E;b[F+22>>1]=~~m;b[F+26>>1]=~~n;b[F+24>>1]=~~o;g[G>>2]=p;g[G+4>>2]=q;g[G+8>>2]=r;g[G+12>>2]=s;g[G+16>>2]=t;g[G+20>>2]=u;g[G+24>>2]=v;g[G+28>>2]=w;g[G+32>>2]=x;g[G+36>>2]=y;g[G+40>>2]=z;g[G+44>>2]=A;g[G+48>>2]=B;g[G+52>>2]=C;c[H>>2]=5360;c[H+4>>2]=2;g[H+8>>2]=.009999999776482582;c[H+148>>2]=0;g[H+12>>2]=0.0;g[H+16>>2]=0.0;De(H,G|0,7);c[F>>2]=H;H=$j(d,F)|0;i=D;return H|0}function ac(d,e,f,j,k,l,m,n,o,p,q,r,s,t,u,v,w,x,y,z,A,B,C,D,E){d=d|0;e=+e;f=+f;j=+j;k=+k;l=+l;m=+m;n=+n;o=+o;p=+p;q=+q;r=+r;s=+s;t=+t;u=+u;v=+v;w=+w;x=+x;y=+y;z=+z;A=+A;B=+B;C=+C;D=+D;E=+E;var F=0,G=0,H=0,I=0,J=0;F=i;i=i+256|0;G=F|0;H=F+8|0;I=F+40|0;J=F+104|0;h[G>>3]=l;g[H+16>>2]=e;g[H+8>>2]=f;a[H+20|0]=j!=0.0|0;g[H+12>>2]=k;c[H+4>>2]=G;b[H+22>>1]=~~m;b[H+26>>1]=~~n;b[H+24>>1]=~~o;g[I>>2]=p;g[I+4>>2]=q;g[I+8>>2]=r;g[I+12>>2]=s;g[I+16>>2]=t;g[I+20>>2]=u;g[I+24>>2]=v;g[I+28>>2]=w;g[I+32>>2]=x;g[I+36>>2]=y;g[I+40>>2]=z;g[I+44>>2]=A;g[I+48>>2]=B;g[I+52>>2]=C;g[I+56>>2]=D;g[I+60>>2]=E;c[J>>2]=5360;c[J+4>>2]=2;g[J+8>>2]=.009999999776482582;c[J+148>>2]=0;g[J+12>>2]=0.0;g[J+16>>2]=0.0;De(J,I|0,8);c[H>>2]=J;J=$j(d,H)|0;i=F;return J|0}function bc(b,d,e,f,j,k,l,m,n,o,p,q,r,s,t,u,v,w,x,y,z,A,B,C,D,E,F,G){b=b|0;d=+d;e=+e;f=+f;j=+j;k=+k;l=+l;m=+m;n=+n;o=+o;p=+p;q=+q;r=+r;s=+s;t=+t;u=+u;v=+v;w=+w;x=+x;y=+y;z=+z;A=+A;B=+B;C=+C;D=+D;E=+E;F=+F;G=+G;var H=0,I=0,J=0,K=0,L=0,M=0,N=0,O=0;H=i;i=i+272|0;I=H|0;J=H+8|0;K=H+88|0;L=H+120|0;h[I>>3]=y;M=J+8|0;N=J+20|0;O=J+40|0;Nn(O|0,0,28)|0;g[J+16>>2]=d;g[J+28>>2]=e;a[J+32|0]=~~f;a[J+33|0]=~~j;a[J+34|0]=~~k;a[J+35|0]=~~l;c[J>>2]=~~m;c[J+72>>2]=0;c[J+4>>2]=~~o;g[J+64>>2]=p;p=+q;q=+r;g[N>>2]=p;g[N+4>>2]=q;q=+s;s=+t;g[M>>2]=q;g[M+4>>2]=s;c[J+60>>2]=0;c[J+56>>2]=~~v;c[J+48>>2]=0;c[J+44>>2]=0;g[J+36>>2]=w;g[J+52>>2]=x;c[J+68>>2]=I;g[K>>2]=z;g[K+4>>2]=A;g[K+8>>2]=B;g[K+12>>2]=C;g[K+16>>2]=D;g[K+20>>2]=E;g[K+24>>2]=F;g[K+28>>2]=G;c[L>>2]=5360;c[L+4>>2]=2;g[L+8>>2]=.009999999776482582;c[L+148>>2]=0;g[L+12>>2]=0.0;g[L+16>>2]=0.0;De(L,K|0,4);c[O>>2]=L;L=Uk(b,J)|0;i=H;return L|0}function cc(a,b,d,e,f,h,j,k,l,m,n,o,p){a=a|0;b=+b;d=+d;e=+e;f=+f;h=+h;j=+j;k=+k;l=+l;m=+m;n=+n;o=+o;p=+p;var q=0,r=0,s=0,t=0;q=i;i=i+200|0;r=q|0;s=q+32|0;t=q+184|0;g[r>>2]=b;g[r+4>>2]=d;g[r+8>>2]=e;g[r+12>>2]=f;g[r+16>>2]=h;g[r+20>>2]=j;g[r+24>>2]=k;g[r+28>>2]=l;c[s>>2]=5360;c[s+4>>2]=2;g[s+8>>2]=.009999999776482582;c[s+148>>2]=0;g[s+12>>2]=0.0;g[s+16>>2]=0.0;De(s,r|0,4);g[t>>2]=m;g[t+4>>2]=n;g[t+8>>2]=o;g[t+12>>2]=p;p=+(Rk(a,s|0,t,0)|0);i=q;return+p}function dc(a,d,e){a=a|0;d=+d;e=+e;var f=0.0,h=0,i=0,j=0,k=0;f=d;if((c[a>>2]|0)!=2){return}h=a+4|0;i=b[h>>1]|0;do{if(e!=0.0){if(!((i&2)==0)){j=i;break}k=i|2;b[h>>1]=k;g[a+160>>2]=0.0;j=k}else{j=i}}while(0);if((j&2)==0){return}j=a+88|0;g[j>>2]=+g[j>>2]+f*+g[a+144>>2];return}function ec(a,d,e,f,h,i){a=a|0;d=+d;e=+e;f=+f;h=+h;i=+i;var j=0.0,k=0,l=0,m=0,n=0;j=d;d=e;e=f;f=h;if((c[a>>2]|0)!=2){return}k=a+4|0;l=b[k>>1]|0;do{if(i!=0.0){if(!((l&2)==0)){m=l;break}n=l|2;b[k>>1]=n;g[a+160>>2]=0.0;m=n}else{m=l}}while(0);if((m&2)==0){return}m=a+92|0;g[m>>2]=j+ +g[m>>2];m=a+96|0;g[m>>2]=d+ +g[m>>2];m=a+100|0;g[m>>2]=+g[m>>2]+(d*(e- +g[a+60>>2])-j*(f- +g[a+64>>2]));return}function fc(a,d,e,f){a=a|0;d=+d;e=+e;f=+f;var h=0.0,i=0,j=0,k=0,l=0;h=d;d=e;if((c[a>>2]|0)!=2){return}i=a+4|0;j=b[i>>1]|0;do{if(f!=0.0){if(!((j&2)==0)){k=j;break}l=j|2;b[i>>1]=l;g[a+160>>2]=0.0;k=l}else{k=j}}while(0);if((k&2)==0){return}k=a+92|0;g[k>>2]=h+ +g[k>>2];k=a+96|0;g[k>>2]=d+ +g[k>>2];return}function gc(a,d,e){a=a|0;d=+d;e=+e;var f=0.0,h=0,i=0,j=0,k=0;f=d;if((c[a>>2]|0)!=2){return}h=a+4|0;i=b[h>>1]|0;do{if(e!=0.0){if(!((i&2)==0)){j=i;break}k=i|2;b[h>>1]=k;g[a+160>>2]=0.0;j=k}else{j=i}}while(0);if((j&2)==0){return}j=a+100|0;g[j>>2]=f+ +g[j>>2];return}function hc(a,b){a=a|0;b=b|0;ak(a,b);return}function ic(a){a=a|0;return+(+g[a+72>>2])}function jc(a){a=a|0;return+(+g[a+88>>2])}function kc(a){a=a|0;var b=0.0,c=0.0;b=+g[a+44>>2];c=+g[a+48>>2];return+(+g[a+140>>2]+ +g[a+132>>2]*(b*b+c*c))}function lc(a,b){a=a|0;b=b|0;var c=0.0;c=+g[a+84>>2];g[b>>2]=+g[a+80>>2];g[b+4>>2]=c;return}function mc(a,b,c,d){a=a|0;b=+b;c=+c;d=d|0;var e=0.0,f=0.0;e=b- +g[a+12>>2];b=c- +g[a+16>>2];c=+g[a+24>>2];f=+g[a+20>>2];g[d>>2]=e*c+b*f;g[d+4>>2]=c*b+e*(-0.0-f);return}function nc(a,b,c,d){a=a|0;b=+b;c=+c;d=d|0;var e=0.0,f=0.0;e=b- +g[a+12>>2];b=c- +g[a+16>>2];c=+g[a+24>>2];f=+g[a+20>>2];g[d>>2]=e*c+b*f;g[d+4>>2]=c*b+e*(-0.0-f);return}function oc(a){a=a|0;return+(+g[a+132>>2])}function pc(a,b){a=a|0;b=b|0;var c=0.0;c=+g[a+16>>2];g[b>>2]=+g[a+12>>2];g[b+4>>2]=c;return}function qc(a,b){a=a|0;b=b|0;g[b>>2]=+g[a+12>>2];g[b+4>>2]=+g[a+16>>2];g[b+8>>2]=+g[a+20>>2];g[b+12>>2]=+g[a+24>>2];return}function rc(a){a=a|0;return+(+((c[a>>2]|0)>>>0>>>0))}function sc(a,b){a=a|0;b=b|0;var c=0.0;c=+g[a+64>>2];g[b>>2]=+g[a+60>>2];g[b+4>>2]=c;return}function tc(a,b,c,d){a=a|0;b=+b;c=+c;d=d|0;var e=0.0,f=0.0,h=0.0;e=b;b=c;c=+g[a+24>>2];f=+g[a+20>>2];h=b*c+e*f+ +g[a+16>>2];g[d>>2]=+g[a+12>>2]+(e*c-b*f);g[d+4>>2]=h;return}function uc(a,b,c,d){a=a|0;b=+b;c=+c;d=d|0;var e=0.0,f=0.0,h=0.0;e=b;b=c;c=+g[a+24>>2];f=+g[a+20>>2];h=b*c+e*f+ +g[a+16>>2];g[d>>2]=+g[a+12>>2]+(e*c-b*f);g[d+4>>2]=h;return}function vc(a,c){a=a|0;c=+c;var d=0,e=0;d=a+4|0;e=b[d>>1]|0;if(!(c!=0.0)){b[d>>1]=e&-3;g[a+160>>2]=0.0;Nn(a+80|0,0,24)|0;return}if(!((e&2)==0)){return}b[d>>1]=e|2;g[a+160>>2]=0.0;return}function wc(a,d){a=a|0;d=+d;var e=0.0,f=0,h=0;e=d;if((c[a>>2]|0)==0){return}do{if(e*e>0.0){f=a+4|0;h=b[f>>1]|0;if(!((h&2)==0)){break}b[f>>1]=h|2;g[a+160>>2]=0.0}}while(0);g[a+88>>2]=e;return}function xc(a,d,e){a=a|0;d=+d;e=+e;var f=0.0,h=0,i=0;f=d;d=e;if((c[a>>2]|0)==0){return}do{if(f*f+d*d>0.0){h=a+4|0;i=b[h>>1]|0;if(!((i&2)==0)){break}b[h>>1]=i|2;g[a+160>>2]=0.0}}while(0);i=a+80|0;e=+f;f=+d;g[i>>2]=e;g[i+4>>2]=f;return}function yc(a,b,c,d,e){a=a|0;b=+b;c=+c;d=+d;e=+e;var f=0,h=0;f=i;i=i+16|0;h=f|0;g[h>>2]=b;g[h+4>>2]=c;g[h+8>>2]=d;g[h+12>>2]=e;bk(a,h);i=f;return}function zc(a,b,c,d){a=a|0;b=+b;c=+c;d=+d;var e=0,f=0;e=i;i=i+8|0;f=e|0;g[f>>2]=b;g[f+4>>2]=c;dk(a,f,d);i=e;return}function Ac(a,b){a=a|0;b=+b;Yj(a,~~b);return}function Bc(a,b,d){a=a|0;b=+b;d=+d;var e=0,f=0,h=0;e=i;i=i+8|0;f=e|0;g[f>>2]=b;g[f+4>>2]=d;h=c[a+12>>2]|0;d=+(((lb[c[(c[h>>2]|0)+16>>2]&31](h,(c[a+8>>2]|0)+12|0,f)|0)&1)>>>0);i=e;return+d}function Cc(a){a=a|0;return}function Dc(a){a=a|0;return}function Ec(a){a=a|0;return}function Fc(a,b){a=+a;b=+b;var c=0,d=0,e=0;c=i;i=i+8|0;d=c|0;e=Hn(103048)|0;g[d>>2]=a;g[d+4>>2]=b;Gj(e,d);i=c;return e|0}function Gc(b,d,e,f,h,j,k,l,m,n,o,p,q,r,s,t,u){b=b|0;d=+d;e=+e;f=+f;h=+h;j=+j;k=+k;l=+l;m=+m;n=+n;o=+o;p=+p;q=+q;r=+r;s=+s;t=+t;u=u|0;var v=0,w=0,x=0;v=i;i=i+56|0;w=v|0;x=w+4|0;Nn(x|0,0,24)|0;a[w+40|0]=d!=0.0|0;a[w+36|0]=e!=0.0|0;g[w+12>>2]=f;g[w+24>>2]=h;g[w+32>>2]=j;a[w+37|0]=k!=0.0|0;a[w+39|0]=l!=0.0|0;a[w+38|0]=m!=0.0|0;g[w+48>>2]=n;g[w+28>>2]=o;g[w+16>>2]=p;g[w+20>>2]=q;g[x>>2]=r;g[w+8>>2]=s;c[w>>2]=~~t;c[w+44>>2]=u;u=Kj(b,w)|0;i=v;return u|0}function Hc(b,d,e,f,h,j,k,l,m,n,o,p,q,r,s,t,u,v){b=b|0;d=+d;e=+e;f=+f;h=+h;j=+j;k=+k;l=+l;m=+m;n=+n;o=+o;p=+p;q=+q;r=+r;s=+s;t=+t;u=+u;v=+v;var w=0,x=0,y=0;w=i;i=i+88|0;x=w|0;a[x|0]=0;g[x+4>>2]=1.0;g[x+8>>2]=1.0;c[x+16>>2]=0;g[x+72>>2]=d;g[x+24>>2]=e;a[x+76|0]=f!=0.0|0;g[x+56>>2]=h;g[x+28>>2]=j;g[x+80>>2]=k;g[x+52>>2]=l;g[x+20>>2]=m;g[x+12>>2]=n;g[x+48>>2]=o;g[x+32>>2]=p;c[x+68>>2]=~~q;g[x+64>>2]=r;g[x+60>>2]=s;g[x+44>>2]=t;g[x+40>>2]=u;g[x+36>>2]=v;y=Oj(b,x)|0;i=w;return y|0}function Ic(a){a=a|0;if((a|0)==0){return}Hj(a);Jn(a);return}function Jc(a,b){a=a|0;b=b|0;Lj(a,b);return}function Kc(a,b){a=a|0;b=b|0;Mj(a,b);return}function Lc(a,b){a=a|0;b=b|0;Ij(a,b);return}function Mc(a,b,c,d,e){a=a|0;b=+b;c=+c;d=+d;e=+e;var f=0,h=0,j=0,k=0.0;f=i;i=i+16|0;h=f|0;j=h|0;k=+b;b=+c;g[j>>2]=k;g[j+4>>2]=b;j=h+8|0;b=+d;d=+e;g[j>>2]=b;g[j+4>>2]=d;Sj(a,8496,h);i=f;return}function Nc(a,b,c,d,e){a=a|0;b=+b;c=+c;d=+d;e=+e;var f=0,h=0,j=0;f=i;i=i+16|0;h=f|0;j=f+8|0;g[h>>2]=b;g[h+4>>2]=c;g[j>>2]=d;g[j+4>>2]=e;Tj(a,8488,h,j);i=f;return}function Oc(a){a=a|0;Jj(a,8528);return}function Pc(a,b,c){a=a|0;b=+b;c=+c;var d=0,e=0.0;d=a+102980|0;e=+b;b=+c;g[d>>2]=e;g[d+4>>2]=b;return}function Qc(a,b,c,d){a=a|0;b=+b;c=+c;d=+d;Rj(a,b,~~c,~~d,3);return}function Rc(a){a=a|0;return a+64|0}function Sc(a){a=a|0;var b=0,d=0;b=c[a+48>>2]|0;d=c[a+52>>2]|0;yf(8456,a+64|0,(c[b+8>>2]|0)+12|0,+g[(c[b+12>>2]|0)+8>>2],(c[d+8>>2]|0)+12|0,+g[(c[d+12>>2]|0)+8>>2]);return 8456}function Tc(b,d,e,f,h,j,k,l,m,n,o){b=b|0;d=d|0;e=e|0;f=+f;h=+h;j=+j;k=+k;l=+l;m=+m;n=+n;o=+o;var p=0,q=0,r=0;p=i;i=i+48|0;q=p|0;Nn(q|0,0,16)|0;c[q>>2]=3;r=q+20|0;c[r>>2]=0;c[r+4>>2]=0;c[q+8>>2]=d;c[q+12>>2]=e;a[q+16|0]=f!=0.0|0;g[q+44>>2]=h;g[q+40>>2]=j;g[q+36>>2]=k;e=q+20|0;k=+l;l=+m;g[e>>2]=k;g[e+4>>2]=l;e=q+28|0;l=+n;n=+o;g[e>>2]=l;g[e+4>>2]=n;e=Nj(b,q|0)|0;i=p;return e|0}function Uc(b,d,e,f,h,j,k,l,m,n){b=b|0;d=d|0;e=e|0;f=+f;h=+h;j=+j;k=+k;l=+l;m=+m;n=+n;var o=0,p=0,q=0,r=0;o=i;i=i+64|0;p=o|0;q=o+48|0;r=o+56|0;Nn(p|0,0,16)|0;c[p>>2]=3;Nn(p+20|0,0,16)|0;g[p+36>>2]=1.0;a[p+16|0]=l!=0.0|0;g[p+44>>2]=m;g[p+40>>2]=n;g[q>>2]=f;g[q+4>>2]=h;g[r>>2]=j;g[r+4>>2]=k;kg(p,d,e,q,r);r=Nj(b,p|0)|0;i=o;return r|0}function Vc(b,d,e,f,h,j,k,l,m,n){b=b|0;d=d|0;e=e|0;f=+f;h=+h;j=+j;k=+k;l=+l;m=+m;n=+n;var o=0,p=0;o=i;i=i+48|0;p=o|0;Nn(p|0,0,16)|0;c[p>>2]=9;Nn(p+20|0,0,20)|0;c[p+8>>2]=d;c[p+12>>2]=e;a[p+16|0]=f!=0.0|0;e=p+20|0;f=+h;h=+j;g[e>>2]=f;g[e+4>>2]=h;e=p+28|0;h=+k;k=+l;g[e>>2]=h;g[e+4>>2]=k;g[p+36>>2]=m;g[p+40>>2]=n;e=Nj(b,p|0)|0;i=o;return e|0}function Wc(b,d,e,f,h,j,k,l){b=b|0;d=d|0;e=e|0;f=+f;h=+h;j=+j;k=+k;l=+l;var m=0,n=0,o=0;m=i;i=i+56|0;n=m|0;o=m+48|0;Nn(n|0,0,16)|0;c[n>>2]=9;Nn(n+20|0,0,20)|0;a[n+16|0]=j!=0.0|0;g[n+36>>2]=k;g[n+40>>2]=l;g[o>>2]=f;g[o+4>>2]=h;wg(n,d,e,o);o=Nj(b,n|0)|0;i=m;return o|0}function Xc(a){a=a|0;return+(+ah(a))}function Yc(b,d,e,f,h,j,k){b=b|0;d=d|0;e=e|0;f=+f;h=h|0;j=j|0;k=+k;var l=0,m=0;l=i;i=i+32|0;m=l|0;Nn(m|0,0,16)|0;c[m>>2]=6;c[m+8>>2]=d;c[m+12>>2]=e;a[m+16|0]=f!=0.0|0;c[m+20>>2]=h;c[m+24>>2]=j;g[m+28>>2]=k;j=Nj(b,m|0)|0;i=l;return j|0}function Zc(a){a=a|0;return c[a+48>>2]|0}function _c(a){a=a|0;return c[a+52>>2]|0}function $c(a,b){a=a|0;b=+b;fg(a,b);return}function ad(a,b,c){a=a|0;b=+b;c=+c;var d=0,e=0;d=i;i=i+8|0;e=d|0;g[e>>2]=b;g[e+4>>2]=c;eg(a,e);i=d;return}function bd(b,d,e,f,h,j,k,l,m,n){b=b|0;d=d|0;e=e|0;f=+f;h=+h;j=+j;k=+k;l=+l;m=+m;n=+n;var o=0,p=0;o=i;i=i+48|0;p=o|0;Nn(p|0,0,16)|0;c[p>>2]=11;c[p+8>>2]=d;c[p+12>>2]=e;a[p+16|0]=f!=0.0|0;g[p+28>>2]=h;g[p+40>>2]=j;e=p+20|0;j=+k;k=+l;g[e>>2]=j;g[e+4>>2]=k;g[p+32>>2]=m;g[p+36>>2]=n;e=Nj(b,p|0)|0;i=o;return e|0}function cd(b,d,e,f,h,j,k){b=b|0;d=d|0;e=e|0;f=+f;h=+h;j=+j;k=+k;var l=0,m=0;l=i;i=i+48|0;m=l|0;Nn(m|0,0,16)|0;c[m>>2]=11;g[m+20>>2]=0.0;g[m+24>>2]=0.0;g[m+28>>2]=0.0;a[m+16|0]=f!=0.0|0;g[m+40>>2]=h;g[m+32>>2]=j;g[m+36>>2]=k;Xf(m,d,e);e=Nj(b,m|0)|0;i=l;return e|0}function dd(a,b,c){a=a|0;b=+b;c=+c;var d=0,e=0;d=i;i=i+8|0;e=d|0;g[e>>2]=b;g[e+4>>2]=c;Lf(a,e);i=d;return}function ed(b,d,e,f,h,j,k,l,m){b=b|0;d=d|0;e=e|0;f=+f;h=+h;j=+j;k=+k;l=+l;m=+m;var n=0,o=0;n=i;i=i+40|0;o=n|0;Nn(o|0,0,16)|0;c[o>>2]=5;c[o+8>>2]=d;c[o+12>>2]=e;a[o+16|0]=f!=0.0|0;g[o+36>>2]=h;g[o+32>>2]=j;g[o+28>>2]=k;e=o+20|0;k=+l;l=+m;g[e>>2]=k;g[e+4>>2]=l;e=Nj(b,o|0)|0;i=n;return e|0}function fd(a,b){a=a|0;b=+b;ph(a,b!=0.0);return}function gd(a,b){a=a|0;b=+b;rh(a,b!=0.0);return}function hd(a){a=a|0;return+(+nh(a))}function id(a){a=a|0;return+(+g[a+132>>2])}function jd(a,b){a=a|0;b=+b;return+(+th(a,b))}function kd(a){a=a|0;return+(+(((oh(a)|0)&1)>>>0))}function ld(a){a=a|0;return+(+(((qh(a)|0)&1)>>>0))}function md(a,b){a=a|0;b=+b;sh(a,b);return}function nd(b,d,e,f,h,j,k,l,m,n,o,p,q,r,s,t,u){b=b|0;d=d|0;e=e|0;f=+f;h=+h;j=+j;k=+k;l=+l;m=+m;n=+n;o=+o;p=+p;q=+q;r=+r;s=+s;t=+t;u=+u;var v=0,w=0,x=0;v=i;i=i+72|0;w=v|0;Nn(w|0,0,16)|0;c[w>>2]=2;x=w+20|0;c[x>>2]=0;c[x+4>>2]=0;c[w+8>>2]=d;c[w+12>>2]=e;a[w+16|0]=f!=0.0|0;a[w+48|0]=h!=0.0|0;a[w+60|0]=j!=0.0|0;e=w+20|0;j=+k;k=+l;g[e>>2]=j;g[e+4>>2]=k;e=w+28|0;k=+m;m=+n;g[e>>2]=k;g[e+4>>2]=m;e=w+36|0;m=+o;o=+p;g[e>>2]=m;g[e+4>>2]=o;g[w+52>>2]=q;g[w+64>>2]=r;g[w+68>>2]=s;g[w+44>>2]=t;g[w+56>>2]=u;e=Nj(b,w|0)|0;i=v;return e|0}function od(b,d,e,f,h,j,k,l,m,n,o,p,q,r){b=b|0;d=d|0;e=e|0;f=+f;h=+h;j=+j;k=+k;l=+l;m=+m;n=+n;o=+o;p=+p;q=+q;r=+r;var s=0,t=0,u=0,v=0;s=i;i=i+88|0;t=s|0;u=s+72|0;v=s+80|0;Nn(t|0,0,16)|0;c[t>>2]=2;Nn(t+20|0,0,16)|0;g[t+36>>2]=1.0;g[t+40>>2]=0.0;g[t+44>>2]=0.0;a[t+16|0]=l!=0.0|0;a[t+48|0]=m!=0.0|0;a[t+60|0]=n!=0.0|0;g[t+52>>2]=o;g[t+64>>2]=p;g[t+68>>2]=q;g[t+56>>2]=r;g[u>>2]=f;g[u+4>>2]=h;g[v>>2]=j;g[v+4>>2]=k;eh(t,d,e,u,v);v=Nj(b,t|0)|0;i=s;return v|0}function pd(b,d,e,f,h,j,k,l,m,n,o,p,q,r,s){b=b|0;d=d|0;e=e|0;f=+f;h=+h;j=+j;k=+k;l=+l;m=+m;n=+n;o=+o;p=+p;q=+q;r=+r;s=+s;var t=0,u=0;t=i;i=i+64|0;u=t|0;Nn(u|0,0,12)|0;c[u>>2]=4;c[u+8>>2]=d;c[u+12>>2]=e;a[u+16|0]=f!=0.0|0;e=u+20|0;f=+h;h=+j;g[e>>2]=f;g[e+4>>2]=h;e=u+28|0;h=+k;k=+l;g[e>>2]=h;g[e+4>>2]=k;g[u+52>>2]=m;g[u+56>>2]=n;e=u+36|0;n=+o;o=+p;g[e>>2]=n;g[e+4>>2]=o;e=u+44|0;o=+q;q=+r;g[e>>2]=o;g[e+4>>2]=q;g[u+60>>2]=s;e=Nj(b,u|0)|0;i=t;return e|0}function qd(b,d,e,f,h,j,k,l,m,n,o,p,q){b=b|0;d=d|0;e=e|0;f=+f;h=+h;j=+j;k=+k;l=+l;m=+m;n=+n;o=+o;p=+p;q=+q;var r=0,s=0,t=0,u=0,v=0,w=0;r=i;i=i+96|0;s=r|0;t=r+64|0;u=r+72|0;v=r+80|0;w=r+88|0;Nn(s|0,0,16)|0;c[s>>2]=4;g[s+20>>2]=-1.0;g[s+24>>2]=1.0;g[s+28>>2]=1.0;g[s+32>>2]=1.0;g[s+36>>2]=-1.0;g[s+40>>2]=0.0;g[s+44>>2]=1.0;g[s+48>>2]=0.0;g[s+52>>2]=0.0;g[s+56>>2]=0.0;g[s+60>>2]=1.0;a[s+16|0]=q!=0.0|0;g[t>>2]=l;g[t+4>>2]=m;g[u>>2]=n;g[u+4>>2]=o;g[v>>2]=f;g[v+4>>2]=h;g[w>>2]=j;g[w+4>>2]=k;ri(s,d,e,t,u,v,w,p);w=Nj(b,s|0)|0;i=r;return w|0}function rd(a,b){a=a|0;b=+b;ai(a,b!=0.0);return}function sd(a,b){a=a|0;b=+b;Zh(a,b!=0.0);return}function td(a){a=a|0;return+(+Xh(a))}function ud(a){a=a|0;return+(+((($h(a)|0)&1)>>>0))}function vd(a){a=a|0;return+(+(((Yh(a)|0)&1)>>>0))}function wd(b,d,e,f,h,j,k,l,m,n,o,p,q,r,s){b=b|0;d=d|0;e=e|0;f=+f;h=+h;j=+j;k=+k;l=+l;m=+m;n=+n;o=+o;p=+p;q=+q;r=+r;s=+s;var t=0,u=0;t=i;i=i+64|0;u=t|0;Nn(u|0,0,16)|0;c[u>>2]=1;Nn(u+20|0,0,16)|0;c[u+8>>2]=d;c[u+12>>2]=e;a[u+16|0]=f!=0.0|0;a[u+40|0]=h!=0.0|0;a[u+52|0]=j!=0.0|0;e=u+20|0;j=+l;l=+m;g[e>>2]=j;g[e+4>>2]=l;e=u+28|0;l=+n;n=+o;g[e>>2]=l;g[e+4>>2]=n;g[u+44>>2]=k;g[u+60>>2]=p;g[u+56>>2]=q;g[u+36>>2]=r;g[u+48>>2]=s;e=Nj(b,u|0)|0;i=t;return e|0}function xd(b,d,e,f,h,j,k,l,m,n,o,p){b=b|0;d=d|0;e=e|0;f=+f;h=+h;j=+j;k=+k;l=+l;m=+m;n=+n;o=+o;p=+p;var q=0,r=0,s=0;q=i;i=i+72|0;r=q|0;s=q+64|0;Nn(r|0,0,16)|0;c[r>>2]=1;Nn(r+20|0,0,20)|0;a[r+16|0]=j!=0.0|0;a[r+40|0]=k!=0.0|0;a[r+52|0]=l!=0.0|0;g[r+44>>2]=m;g[r+60>>2]=n;g[r+56>>2]=o;g[r+48>>2]=p;g[s>>2]=f;g[s+4>>2]=h;Oh(r,d,e,s);s=Nj(b,r|0)|0;i=q;return s|0}function yd(a,b){a=a|0;b=+b;_h(a,b);return}function zd(b,d,e,f,h,j,k,l,m){b=b|0;d=d|0;e=e|0;f=+f;h=+h;j=+j;k=+k;l=+l;m=+m;var n=0,o=0;n=i;i=i+40|0;o=n|0;Nn(o|0,0,16)|0;c[o>>2]=10;c[o+8>>2]=d;c[o+12>>2]=e;a[o+16|0]=f!=0.0|0;e=o+20|0;f=+h;h=+j;g[e>>2]=f;g[e+4>>2]=h;e=o+28|0;h=+k;k=+l;g[e>>2]=h;g[e+4>>2]=k;g[o+36>>2]=m;e=Nj(b,o|0)|0;i=n;return e|0}function Ad(b,d,e,f,h,j,k,l,m,n,o){b=b|0;d=d|0;e=e|0;f=+f;h=+h;j=+j;k=+k;l=+l;m=+m;n=+n;o=+o;var p=0,q=0;p=i;i=i+48|0;q=p|0;Nn(q|0,0,16)|0;c[q>>2]=8;Nn(q+20|0,0,16)|0;c[q+8>>2]=d;c[q+12>>2]=e;a[q+16|0]=f!=0.0|0;g[q+44>>2]=h;g[q+40>>2]=j;e=q+20|0;j=+k;k=+l;g[e>>2]=j;g[e+4>>2]=k;e=q+28|0;k=+m;m=+n;g[e>>2]=k;g[e+4>>2]=m;g[q+36>>2]=o;e=Nj(b,q|0)|0;i=p;return e|0}function Bd(b,d,e,f,h,j,k,l){b=b|0;d=d|0;e=e|0;f=+f;h=+h;j=+j;k=+k;l=+l;var m=0,n=0,o=0;m=i;i=i+56|0;n=m|0;o=m+48|0;Nn(n|0,0,16)|0;c[n>>2]=8;Nn(n+20|0,0,20)|0;a[n+16|0]=j!=0.0|0;g[n+44>>2]=k;g[n+40>>2]=l;g[o>>2]=f;g[o+4>>2]=h;Ig(n,d,e,o);o=Nj(b,n|0)|0;i=m;return o|0}function Cd(a,b){a=a|0;b=+b;ni(a,b);return}function Dd(a,b){a=a|0;b=+b;g[a+68>>2]=b;return}function Ed(b,d,e,f,h,j,k,l,m,n,o,p,q,r,s){b=b|0;d=d|0;e=e|0;f=+f;h=+h;j=+j;k=+k;l=+l;m=+m;n=+n;o=+o;p=+p;q=+q;r=+r;s=+s;var t=0,u=0,v=0;t=i;i=i+64|0;u=t|0;Nn(u|0,0,16)|0;c[u>>2]=7;v=u+20|0;c[v>>2]=0;c[v+4>>2]=0;c[u+8>>2]=d;c[u+12>>2]=e;a[u+16|0]=f!=0.0|0;g[u+60>>2]=h;a[u+44|0]=j!=0.0|0;g[u+56>>2]=k;e=u+20|0;k=+l;l=+m;g[e>>2]=k;g[e+4>>2]=l;e=u+28|0;l=+n;n=+o;g[e>>2]=l;g[e+4>>2]=n;e=u+36|0;n=+p;p=+q;g[e>>2]=n;g[e+4>>2]=p;g[u+48>>2]=r;g[u+52>>2]=s;e=Nj(b,u|0)|0;i=t;return e|0}function Fd(b,d,e,f,h,j,k,l,m,n,o,p,q){b=b|0;d=d|0;e=e|0;f=+f;h=+h;j=+j;k=+k;l=+l;m=+m;n=+n;o=+o;p=+p;q=+q;var r=0,s=0,t=0,u=0;r=i;i=i+80|0;s=r|0;t=r+64|0;u=r+72|0;Nn(s|0,0,16)|0;c[s>>2]=7;Nn(s+20|0,0,16)|0;g[s+36>>2]=1.0;g[s+40>>2]=0.0;a[s+16|0]=l!=0.0|0;g[s+60>>2]=m;a[s+44|0]=n!=0.0|0;g[s+56>>2]=o;g[s+48>>2]=p;g[s+52>>2]=q;g[t>>2]=f;g[t+4>>2]=h;g[u>>2]=j;g[u+4>>2]=k;ei(s,d,e,t,u);u=Nj(b,s|0)|0;i=r;return u|0}function Gd(a,b,c){a=a|0;b=+b;c=+c;var d=0,e=0;d=i;i=i+8|0;e=d|0;g[e>>2]=b;g[e+4>>2]=c;Dk(a,e);i=d;return}function Hd(a,b,c){a=a|0;b=+b;c=+c;var d=0,e=0;d=i;i=i+8|0;e=d|0;g[e>>2]=b;g[e+4>>2]=c;Ek(a,e);i=d;return}function Id(a,b){a=a|0;b=+b;Fk(a,b!=0.0);return}function Jd(a){a=a|0;return+(+(c[a+4>>2]|0))}function Kd(a){a=a|0;return+(+((c[a+8>>2]|0)-(c[a+4>>2]|0)|0))}function Ld(b,d,e,f,j,k,l,m,n,o,p,q,r){b=b|0;d=+d;e=+e;f=+f;j=+j;k=+k;l=+l;m=+m;n=+n;o=+o;p=+p;q=+q;r=+r;var s=0,t=0,u=0,v=0,w=0,x=0;s=i;i=i+56|0;t=s|0;u=s+8|0;v=s+16|0;h[t>>3]=l;h[u>>3]=p;w=v+4|0;x=v+12|0;a[v+20|0]=~~d;a[v+21|0]=~~f;a[v+22|0]=~~e;a[v+23|0]=~~j;c[v>>2]=~~k;c[v+32>>2]=t;g[v+24>>2]=m;m=+n;n=+o;g[w>>2]=m;g[w+4>>2]=n;c[v+28>>2]=u;n=+q;q=+r;g[x>>2]=n;g[x+4>>2]=q;q=+(Lk(b,v)|0);i=s;return+q}function Md(a){a=a|0;return Jk(a)|0}function Nd(a){a=a|0;return+(+(c[a+44>>2]|0))}function Od(a){a=a|0;return c[a+96>>2]|0}function Pd(a){a=a|0;return c[a+104>>2]|0}function Qd(a,b){a=a|0;b=+b;g[a+340>>2]=b;return}function Rd(a,b){a=a|0;b=+b;var c=0.0;c=b;g[a+320>>2]=c;g[a+28>>2]=1.0/c;return}function Sd(a,b){a=a|0;b=+b;var c=0.0;c=b*2.0;g[a+32>>2]=c;g[a+40>>2]=c*c;g[a+36>>2]=1.0/c;return}function Td(a){a=a|0;Jn(a);return}function Ud(a,b,c,d,e){a=a|0;b=b|0;c=c|0;d=d|0;e=+e;return+(+za(b|0,+(+g[c>>2]),+(+g[c+4>>2]),+(+g[d>>2]),+(+g[d+4>>2]),+e))}function Vd(a,b,c,d,e,f){a=a|0;b=b|0;c=c|0;d=d|0;e=e|0;f=+f;return+0.0}function Wd(a,b){a=a|0;b=b|0;return 1}function Xd(a){a=a|0;Jn(a);return}function Yd(a,b){a=a|0;b=b|0;return Za(b|0)|0}function Zd(a,b,c){a=a|0;b=b|0;c=c|0;return 0}function _d(a,b){a=a|0;b=b|0;return 1}function $d(a){a=a|0;Jn(a);return}function ae(a,b){a=a|0;b=b|0;Ga(b|0);return}function be(a,b){a=a|0;b=b|0;Qa(b|0);return}function ce(a,b,c){a=a|0;b=b|0;c=c|0;return}function de(a,b,c,d){a=a|0;b=b|0;c=c|0;d=d|0;return}function ee(a,b,c){a=a|0;b=b|0;c=c|0;return}function fe(a,b,c,d){a=a|0;b=b|0;c=c|0;d=d|0;return}function ge(a,b,c){a=a|0;b=b|0;c=c|0;Va(b|0,c|0);return}function he(a,b,c){a=a|0;b=b|0;c=c|0;cb(b|0,c|0);return}function ie(a){a=a|0;Jn(a);return}function je(a){a=a|0;Jn(a);return}function ke(a){a=a|0;Jn(a);return}function le(){c[2132]=4776;c[2124]=4920;c[2122]=5320;return}function me(d,e){d=d|0;e=e|0;var f=0,h=0,j=0,k=0,l=0,m=0,n=0,o=0,p=0,q=0,r=0,s=0,t=0,u=0,v=0,w=0.0,x=0.0,y=0.0,z=0,A=0,B=0.0,C=0.0,D=0,E=0.0,F=0.0,G=0,H=0,I=0,J=0,K=0,L=0,M=0,N=0,O=0,Q=0,R=0,S=0,V=0,W=0,X=0,Y=0,Z=0,_=0.0,$=0.0,aa=0,ba=0.0,ca=0.0,da=0.0,ea=0.0,fa=0.0,ga=0.0,ha=0.0,ia=0.0,ja=0.0,ka=0.0,la=0.0,ma=0,na=0,oa=0.0,pa=0,qa=0,ra=0.0,sa=0.0,ta=0,ua=0.0,va=0.0,wa=0.0,xa=0,ya=0.0,za=0.0,Aa=0,Ba=0,Ca=0,Da=0,Ea=0,Fa=0.0,Ga=0.0,Ha=0,Ia=0.0,Ja=0.0,Ka=0.0;f=i;i=i+344|0;h=f|0;j=f+8|0;k=f+48|0;l=f+88|0;m=f+104|0;n=f+200|0;o=f+224|0;p=f+328|0;q=f+336|0;$m(h);c[2142]=(c[2142]|0)+1;r=d|0;c[r>>2]=0;s=e+128|0;t=d+4|0;g[t>>2]=+g[s>>2];d=e|0;u=e+28|0;On(j|0,e+56|0,36)|0;On(k|0,e+92|0,36)|0;v=j+24|0;w=+g[v>>2];x=+P(w/6.2831854820251465)*6.2831854820251465;y=w-x;g[v>>2]=y;z=j+28|0;w=+g[z>>2]-x;g[z>>2]=w;A=k+24|0;x=+g[A>>2];B=+P(x/6.2831854820251465)*6.2831854820251465;C=x-B;g[A>>2]=C;D=k+28|0;x=+g[D>>2]-B;g[D>>2]=x;B=+g[s>>2];E=+g[e+24>>2]+ +g[e+52>>2]+-.014999999664723873;F=E<.004999999888241291?.004999999888241291:E;b[l+4>>1]=0;s=m;G=e;c[s>>2]=c[G>>2];c[s+4>>2]=c[G+4>>2];c[s+8>>2]=c[G+8>>2];c[s+12>>2]=c[G+12>>2];c[s+16>>2]=c[G+16>>2];c[s+20>>2]=c[G+20>>2];c[s+24>>2]=c[G+24>>2];G=m+28|0;s=u;c[G>>2]=c[s>>2];c[G+4>>2]=c[s+4>>2];c[G+8>>2]=c[s+8>>2];c[G+12>>2]=c[s+12>>2];c[G+16>>2]=c[s+16>>2];c[G+20>>2]=c[s+20>>2];c[G+24>>2]=c[s+24>>2];a[m+88|0]=0;s=j+8|0;G=j+12|0;e=j+16|0;H=j+20|0;I=j|0;J=j+4|0;K=k+8|0;L=k+12|0;M=k+16|0;N=k+20|0;O=k|0;Q=k+4|0;R=m+56|0;S=m+64|0;V=m+68|0;W=m+72|0;X=m+80|0;Y=m+84|0;Z=n+16|0;E=F+.0012499999720603228;_=F+-.0012499999720603228;$=0.0;aa=0;ba=y;y=w;w=C;C=x;a:while(1){x=1.0-$;ca=x*ba+$*y;da=+U(ca);ea=+T(ca);ca=+g[I>>2];fa=+g[J>>2];ga=x*w+$*C;ha=+U(ga);ia=+T(ga);ga=+g[O>>2];ja=+g[Q>>2];ka=x*+g[K>>2]+$*+g[M>>2]-(ia*ga-ha*ja);la=x*+g[L>>2]+$*+g[N>>2]-(ha*ga+ia*ja);ja=+(x*+g[s>>2]+$*+g[e>>2]-(ea*ca-da*fa));ga=+(x*+g[G>>2]+$*+g[H>>2]-(da*ca+ea*fa));g[R>>2]=ja;g[R+4>>2]=ga;g[S>>2]=da;g[V>>2]=ea;ea=+ka;ka=+la;g[W>>2]=ea;g[W+4>>2]=ka;g[X>>2]=ha;g[Y>>2]=ia;se(n,l,m);ia=+g[Z>>2];if(!(ia>0.0)){ma=3;break}if(ia<E){ma=5;break}+ne(o,l,d,j,u,k,$);na=0;ia=B;while(1){ha=+oe(o,p,q,ia);if(ha>E){ma=8;break a}if(ha>_){oa=ia;break}pa=c[p>>2]|0;qa=c[q>>2]|0;ka=+pe(o,pa,qa,$);if(ka<_){ma=11;break a}if(ka>E){ra=ia;sa=$;ta=0;ua=ka;va=ha}else{ma=13;break a}while(1){if((ta&1|0)==0){wa=(sa+ra)*.5}else{wa=sa+(F-ua)*(ra-sa)/(va-ua)}xa=ta+1|0;c[2134]=(c[2134]|0)+1;ha=+pe(o,pa,qa,wa);ka=ha-F;if(ka>0.0){ya=ka}else{ya=-0.0-ka}if(ya<.0012499999720603228){za=wa;break}Aa=ha>F;if((xa|0)==50){za=ia;break}else{ra=Aa?ra:wa;sa=Aa?wa:sa;ta=xa;ua=Aa?ha:ua;va=Aa?va:ha}}qa=c[2136]|0;c[2136]=(qa|0)>(xa|0)?qa:xa;qa=na+1|0;if((qa|0)==8){oa=$;break}else{na=qa;ia=za}}na=aa+1|0;c[2140]=(c[2140]|0)+1;if((na|0)==20){ma=25;break}$=oa;aa=na;ba=+g[v>>2];y=+g[z>>2];w=+g[A>>2];C=+g[D>>2]}if((ma|0)==3){c[r>>2]=2;g[t>>2]=0.0;Ba=aa;Ca=c[2138]|0;Da=(Ca|0)>(Ba|0);Ea=Da?Ca:Ba;c[2138]=Ea;Fa=+bn(h);Ga=+g[4];Ha=Ga>Fa;Ia=Ha?Ga:Fa;g[4]=Ia;Ja=+g[2];Ka=Fa+Ja;g[2]=Ka;i=f;return}else if((ma|0)==5){c[r>>2]=3;g[t>>2]=$;Ba=aa;Ca=c[2138]|0;Da=(Ca|0)>(Ba|0);Ea=Da?Ca:Ba;c[2138]=Ea;Fa=+bn(h);Ga=+g[4];Ha=Ga>Fa;Ia=Ha?Ga:Fa;g[4]=Ia;Ja=+g[2];Ka=Fa+Ja;g[2]=Ka;i=f;return}else if((ma|0)==8){c[r>>2]=4;g[t>>2]=B}else if((ma|0)==11){c[r>>2]=1;g[t>>2]=$}else if((ma|0)==13){c[r>>2]=3;g[t>>2]=$}else if((ma|0)==25){c[r>>2]=1;g[t>>2]=oa;Ba=20;Ca=c[2138]|0;Da=(Ca|0)>(Ba|0);Ea=Da?Ca:Ba;c[2138]=Ea;Fa=+bn(h);Ga=+g[4];Ha=Ga>Fa;Ia=Ha?Ga:Fa;g[4]=Ia;Ja=+g[2];Ka=Fa+Ja;g[2]=Ka;i=f;return}c[2140]=(c[2140]|0)+1;Ba=aa+1|0;Ca=c[2138]|0;Da=(Ca|0)>(Ba|0);Ea=Da?Ca:Ba;c[2138]=Ea;Fa=+bn(h);Ga=+g[4];Ha=Ga>Fa;Ia=Ha?Ga:Fa;g[4]=Ia;Ja=+g[2];Ka=Fa+Ja;g[2]=Ka;i=f;return}function ne(e,f,h,i,j,k,l){e=e|0;f=f|0;h=h|0;i=i|0;j=j|0;k=k|0;l=+l;var m=0,n=0,o=0.0,p=0.0,q=0.0,r=0.0,s=0.0,t=0.0,u=0.0,v=0.0,w=0.0,x=0.0,y=0.0,z=0.0,A=0.0,B=0.0,C=0.0,D=0,E=0.0,F=0.0,G=0.0,H=0.0,I=0.0;c[e>>2]=h;c[e+4>>2]=j;m=b[f+4>>1]|0;n=e+8|0;On(n|0,i|0,36)|0;i=e+44|0;On(i|0,k|0,36)|0;o=1.0-l;p=o*+g[e+32>>2]+ +g[e+36>>2]*l;q=+U(p);r=+T(p);p=+g[n>>2];s=+g[e+12>>2];t=o*+g[e+16>>2]+ +g[e+24>>2]*l-(r*p-q*s);u=o*+g[e+20>>2]+ +g[e+28>>2]*l-(q*p+r*s);s=o*+g[e+68>>2]+ +g[e+72>>2]*l;p=+U(s);v=+T(s);s=+g[i>>2];w=+g[e+48>>2];x=o*+g[e+52>>2]+ +g[e+60>>2]*l-(v*s-p*w);y=o*+g[e+56>>2]+ +g[e+64>>2]*l-(p*s+v*w);if(m<<16>>16==1){c[e+80>>2]=0;m=(c[h+16>>2]|0)+(d[f+6|0]<<3)|0;w=+g[m>>2];s=+g[m+4>>2];m=(c[j+16>>2]|0)+(d[f+9|0]<<3)|0;l=+g[m>>2];o=+g[m+4>>2];m=e+92|0;z=x+(v*l-p*o)-(t+(r*w-q*s));A=y+(p*l+v*o)-(u+(q*w+r*s));i=m;s=+z;w=+A;g[i>>2]=s;g[i+4>>2]=w;w=+R(z*z+A*A);if(w<1.1920928955078125e-7){B=0.0}else{s=1.0/w;g[m>>2]=z*s;g[e+96>>2]=A*s;B=w}m=e+84|0;i=8600;n=c[i+4>>2]|0;c[m>>2]=c[i>>2];c[m+4>>2]=n;C=B;return+C}n=f+6|0;m=f+7|0;i=e+80|0;if((a[n]|0)==(a[m]|0)){c[i>>2]=2;k=c[j+16>>2]|0;D=k+(d[f+9|0]<<3)|0;B=+g[D>>2];w=+g[D+4>>2];D=k+(d[f+10|0]<<3)|0;s=+g[D>>2];A=+g[D+4>>2];D=e+92|0;z=A-w;o=(s-B)*-1.0;k=D;l=+z;E=+o;g[k>>2]=l;g[k+4>>2]=E;E=+R(z*z+o*o);if(E<1.1920928955078125e-7){F=z;G=o}else{l=1.0/E;E=z*l;g[D>>2]=E;z=o*l;g[e+96>>2]=z;F=E;G=z}z=(B+s)*.5;s=(w+A)*.5;D=e+84|0;A=+z;w=+s;g[D>>2]=A;g[D+4>>2]=w;D=(c[h+16>>2]|0)+(d[n]<<3)|0;w=+g[D>>2];A=+g[D+4>>2];B=(v*F-p*G)*(t+(r*w-q*A)-(x+(v*z-p*s)))+(p*F+v*G)*(u+(q*w+r*A)-(y+(p*z+v*s)));if(!(B<0.0)){C=B;return+C}s=+(-0.0-F);F=+(-0.0-G);g[k>>2]=s;g[k+4>>2]=F;C=-0.0-B;return+C}else{c[i>>2]=1;i=c[h+16>>2]|0;h=i+(d[n]<<3)|0;B=+g[h>>2];F=+g[h+4>>2];h=i+(d[m]<<3)|0;s=+g[h>>2];G=+g[h+4>>2];h=e+92|0;z=G-F;A=(s-B)*-1.0;m=h;w=+z;E=+A;g[m>>2]=w;g[m+4>>2]=E;E=+R(z*z+A*A);if(E<1.1920928955078125e-7){H=z;I=A}else{w=1.0/E;E=z*w;g[h>>2]=E;z=A*w;g[e+96>>2]=z;H=E;I=z}z=(B+s)*.5;s=(F+G)*.5;h=e+84|0;G=+z;F=+s;g[h>>2]=G;g[h+4>>2]=F;h=(c[j+16>>2]|0)+(d[f+9|0]<<3)|0;F=+g[h>>2];G=+g[h+4>>2];B=(r*H-q*I)*(x+(v*F-p*G)-(t+(r*z-q*s)))+(q*H+r*I)*(y+(p*F+v*G)-(u+(q*z+r*s)));if(!(B<0.0)){C=B;return+C}s=+(-0.0-H);H=+(-0.0-I);g[m>>2]=s;g[m+4>>2]=H;C=-0.0-B;return+C}return 0.0}function oe(a,b,d,e){a=a|0;b=b|0;d=d|0;e=+e;var f=0.0,h=0.0,i=0.0,j=0.0,k=0.0,l=0.0,m=0.0,n=0.0,o=0.0,p=0.0,q=0.0,r=0,s=0.0,t=0.0,u=0.0,v=0,w=0,x=0,y=0,z=0.0,A=0,B=0,C=0,D=0,E=0,F=0.0,G=0,H=0;f=1.0-e;h=f*+g[a+32>>2]+ +g[a+36>>2]*e;i=+U(h);j=+T(h);h=+g[a+8>>2];k=+g[a+12>>2];l=f*+g[a+16>>2]+ +g[a+24>>2]*e-(j*h-i*k);m=f*+g[a+20>>2]+ +g[a+28>>2]*e-(i*h+j*k);k=f*+g[a+68>>2]+ +g[a+72>>2]*e;h=+U(k);n=+T(k);k=+g[a+44>>2];o=+g[a+48>>2];p=f*+g[a+52>>2]+ +g[a+60>>2]*e-(n*k-h*o);q=f*+g[a+56>>2]+ +g[a+64>>2]*e-(h*k+n*o);r=c[a+80>>2]|0;if((r|0)==0){o=+g[a+92>>2];k=+g[a+96>>2];e=j*o+i*k;f=o*(-0.0-i)+j*k;s=-0.0-k;t=n*(-0.0-o)+h*s;u=h*o+n*s;v=c[a>>2]|0;w=c[v+16>>2]|0;x=c[v+20>>2]|0;if((x|0)>1){s=f*+g[w+4>>2]+e*+g[w>>2];v=1;y=0;while(1){z=e*+g[w+(v<<3)>>2]+f*+g[w+(v<<3)+4>>2];A=z>s;B=A?v:y;C=v+1|0;if((C|0)<(x|0)){s=A?z:s;v=C;y=B}else{D=B;break}}}else{D=0}c[b>>2]=D;D=c[a+4>>2]|0;y=c[D+16>>2]|0;v=c[D+20>>2]|0;if((v|0)>1){s=u*+g[y+4>>2]+t*+g[y>>2];D=1;x=0;while(1){f=t*+g[y+(D<<3)>>2]+u*+g[y+(D<<3)+4>>2];B=f>s;C=B?D:x;A=D+1|0;if((A|0)<(v|0)){s=B?f:s;D=A;x=C}else{E=C;break}}}else{E=0}c[d>>2]=E;x=w+(c[b>>2]<<3)|0;s=+g[x>>2];u=+g[x+4>>2];x=y+(E<<3)|0;t=+g[x>>2];f=+g[x+4>>2];F=o*(p+(n*t-h*f)-(l+(j*s-i*u)))+k*(q+(h*t+n*f)-(m+(i*s+j*u)));return+F}else if((r|0)==2){u=+g[a+92>>2];s=+g[a+96>>2];f=n*u-h*s;t=h*u+n*s;s=+g[a+84>>2];u=+g[a+88>>2];k=p+(n*s-h*u);o=q+(h*s+n*u);u=-0.0-t;s=j*(-0.0-f)+i*u;e=i*f+j*u;c[d>>2]=-1;x=c[a>>2]|0;E=c[x+16>>2]|0;y=c[x+20>>2]|0;if((y|0)>1){u=e*+g[E+4>>2]+s*+g[E>>2];x=1;w=0;while(1){z=s*+g[E+(x<<3)>>2]+e*+g[E+(x<<3)+4>>2];D=z>u;v=D?x:w;C=x+1|0;if((C|0)<(y|0)){u=D?z:u;x=C;w=v}else{G=v;break}}}else{G=0}c[b>>2]=G;w=E+(G<<3)|0;u=+g[w>>2];e=+g[w+4>>2];F=f*(l+(j*u-i*e)-k)+t*(m+(i*u+j*e)-o);return+F}else if((r|0)==1){o=+g[a+92>>2];e=+g[a+96>>2];u=j*o-i*e;t=i*o+j*e;e=+g[a+84>>2];o=+g[a+88>>2];k=l+(j*e-i*o);l=m+(i*e+j*o);o=-0.0-t;j=n*(-0.0-u)+h*o;e=h*u+n*o;c[b>>2]=-1;r=c[a+4>>2]|0;a=c[r+16>>2]|0;w=c[r+20>>2]|0;if((w|0)>1){o=e*+g[a+4>>2]+j*+g[a>>2];r=1;G=0;while(1){i=j*+g[a+(r<<3)>>2]+e*+g[a+(r<<3)+4>>2];E=i>o;x=E?r:G;y=r+1|0;if((y|0)<(w|0)){o=E?i:o;r=y;G=x}else{H=x;break}}}else{H=0}c[d>>2]=H;G=a+(H<<3)|0;o=+g[G>>2];e=+g[G+4>>2];F=u*(p+(n*o-h*e)-k)+t*(q+(h*o+n*e)-l);return+F}else{c[b>>2]=-1;c[d>>2]=-1;F=0.0;return+F}return 0.0}function pe(a,b,d,e){a=a|0;b=b|0;d=d|0;e=+e;var f=0.0,h=0.0,i=0.0,j=0.0,k=0.0,l=0.0,m=0.0,n=0.0,o=0.0,p=0.0,q=0.0,r=0,s=0,t=0.0,u=0.0,v=0.0;f=1.0-e;h=f*+g[a+32>>2]+ +g[a+36>>2]*e;i=+U(h);j=+T(h);h=+g[a+8>>2];k=+g[a+12>>2];l=f*+g[a+16>>2]+ +g[a+24>>2]*e-(j*h-i*k);m=f*+g[a+20>>2]+ +g[a+28>>2]*e-(i*h+j*k);k=f*+g[a+68>>2]+ +g[a+72>>2]*e;h=+U(k);n=+T(k);k=+g[a+44>>2];o=+g[a+48>>2];p=f*+g[a+52>>2]+ +g[a+60>>2]*e-(n*k-h*o);q=f*+g[a+56>>2]+ +g[a+64>>2]*e-(h*k+n*o);r=c[a+80>>2]|0;if((r|0)==2){o=+g[a+92>>2];k=+g[a+96>>2];e=+g[a+84>>2];f=+g[a+88>>2];s=(c[(c[a>>2]|0)+16>>2]|0)+(b<<3)|0;t=+g[s>>2];u=+g[s+4>>2];v=(n*o-h*k)*(l+(j*t-i*u)-(p+(n*e-h*f)))+(h*o+n*k)*(m+(i*t+j*u)-(q+(h*e+n*f)));return+v}else if((r|0)==1){f=+g[a+92>>2];e=+g[a+96>>2];u=+g[a+84>>2];t=+g[a+88>>2];s=(c[(c[a+4>>2]|0)+16>>2]|0)+(d<<3)|0;k=+g[s>>2];o=+g[s+4>>2];v=(j*f-i*e)*(p+(n*k-h*o)-(l+(j*u-i*t)))+(i*f+j*e)*(q+(h*k+n*o)-(m+(i*u+j*t)));return+v}else if((r|0)==0){r=(c[(c[a>>2]|0)+16>>2]|0)+(b<<3)|0;t=+g[r>>2];u=+g[r+4>>2];r=(c[(c[a+4>>2]|0)+16>>2]|0)+(d<<3)|0;o=+g[r>>2];k=+g[r+4>>2];v=+g[a+92>>2]*(p+(n*o-h*k)-(l+(j*t-i*u)))+ +g[a+96>>2]*(q+(h*o+n*k)-(m+(i*t+j*u)));return+v}else{v=0.0;return+v}return 0.0}function qe(a,b,d){a=a|0;b=b|0;d=d|0;var e=0,f=0,h=0,i=0,j=0;e=c[b+4>>2]|0;if((e|0)==3){f=b+12|0;h=(c[f>>2]|0)+(d<<3)|0;i=a;j=c[h+4>>2]|0;c[i>>2]=c[h>>2];c[i+4>>2]=j;j=d+1|0;d=a+8|0;i=c[f>>2]|0;if((j|0)<(c[b+16>>2]|0)){f=i+(j<<3)|0;j=d;h=c[f+4>>2]|0;c[j>>2]=c[f>>2];c[j+4>>2]=h}else{h=i;i=d;d=c[h+4>>2]|0;c[i>>2]=c[h>>2];c[i+4>>2]=d}c[a+16>>2]=a;c[a+20>>2]=2;g[a+24>>2]=+g[b+8>>2];return}else if((e|0)==0){c[a+16>>2]=b+12;c[a+20>>2]=1;g[a+24>>2]=+g[b+8>>2];return}else if((e|0)==1){c[a+16>>2]=b+12;c[a+20>>2]=2;g[a+24>>2]=+g[b+8>>2];return}else if((e|0)==2){c[a+16>>2]=b+20;c[a+20>>2]=c[b+148>>2];g[a+24>>2]=+g[b+8>>2];return}else{return}}function re(a){a=a|0;var b=0,d=0.0,e=0.0,f=0,h=0.0,i=0.0,j=0,k=0.0,l=0.0,m=0.0,n=0.0,o=0.0,p=0.0,q=0.0,r=0.0,s=0.0,t=0.0,u=0.0,v=0.0,w=0.0,x=0.0;b=a+16|0;d=+g[b>>2];e=+g[b+4>>2];b=a+36|0;f=a+52|0;h=+g[f>>2];i=+g[f+4>>2];f=a+72|0;j=a+88|0;k=+g[j>>2];l=+g[j+4>>2];m=h-d;n=i-e;o=d*m+e*n;p=h*m+i*n;q=k-d;r=l-e;s=d*q+e*r;t=k*q+l*r;u=k-h;v=l-i;w=h*u+i*v;x=k*u+l*v;v=m*r-n*q;q=(h*l-i*k)*v;n=(e*k-d*l)*v;l=(d*i-e*h)*v;if(!(o<-0.0|s<-0.0)){g[a+24>>2]=1.0;c[a+108>>2]=1;return}if(!(o>=-0.0|p<=0.0|l>0.0)){v=1.0/(p-o);g[a+24>>2]=p*v;g[a+60>>2]=v*(-0.0-o);c[a+108>>2]=2;return}if(!(s>=-0.0|t<=0.0|n>0.0)){o=1.0/(t-s);g[a+24>>2]=t*o;g[a+96>>2]=o*(-0.0-s);c[a+108>>2]=2;On(b|0,f|0,36)|0;return}if(!(p>0.0|w<-0.0)){g[a+60>>2]=1.0;c[a+108>>2]=1;On(a|0,b|0,36)|0;return}if(!(t>0.0|x>0.0)){g[a+96>>2]=1.0;c[a+108>>2]=1;On(a|0,f|0,36)|0;return}if(w>=-0.0|x<=0.0|q>0.0){t=1.0/(l+(q+n));g[a+24>>2]=q*t;g[a+60>>2]=n*t;g[a+96>>2]=l*t;c[a+108>>2]=3;return}else{t=1.0/(x-w);g[a+60>>2]=x*t;g[a+96>>2]=t*(-0.0-w);c[a+108>>2]=2;On(a|0,f|0,36)|0;return}}function se(d,e,f){d=d|0;e=e|0;f=f|0;var h=0,j=0,l=0,m=0,n=0,o=0,p=0,q=0,r=0,s=0,t=0.0,u=0.0,v=0,w=0,x=0.0,y=0.0,z=0.0,A=0.0,B=0.0,C=0,D=0.0,E=0.0,F=0,G=0,H=0,I=0,J=0,K=0,L=0,M=0,N=0,O=0,P=0,Q=0,S=0,T=0.0,U=0.0,V=0.0,W=0.0,X=0.0,Y=0.0,Z=0.0,_=0,$=0,aa=0.0,ba=0.0,ca=0,da=0,ea=0,fa=0,ga=0,ha=0,ia=0,ja=0,ka=0,la=0,ma=0,na=0,oa=0,pa=0,qa=0,ra=0,sa=0.0,ta=0.0,ua=0.0,va=0.0,wa=0,xa=0,ya=0,za=0.0,Aa=0,Ba=0.0,Ca=0.0;h=i;i=i+144|0;j=h|0;l=h+16|0;m=h+32|0;n=m|0;o=m;p=i;i=i+12|0;i=i+7&-8;q=i;i=i+12|0;i=i+7&-8;c[2148]=(c[2148]|0)+1;r=j;s=f+56|0;c[r>>2]=c[s>>2];c[r+4>>2]=c[s+4>>2];c[r+8>>2]=c[s+8>>2];c[r+12>>2]=c[s+12>>2];s=l;r=f+72|0;c[s>>2]=c[r>>2];c[s+4>>2]=c[r+4>>2];c[s+8>>2]=c[r+8>>2];c[s+12>>2]=c[r+12>>2];te(o,e,f|0,j,f+28|0,l);r=m;Nn(p|0,0,12)|0;Nn(q|0,0,12)|0;s=o+108|0;t=+g[j+12>>2];u=+g[j+8>>2];v=f+16|0;w=f+20|0;x=+g[j>>2];y=+g[j+4>>2];z=+g[l+12>>2];A=+g[l+8>>2];B=-0.0-A;j=f+44|0;C=f+48|0;D=+g[l>>2];E=+g[l+4>>2];l=m+16|0;F=l;G=o+20|0;H=o+52|0;I=m+56|0;J=o+52|0;K=m+24|0;L=o+60|0;M=m;N=o+36|0;O=0;P=c[s>>2]|0;a:while(1){Q=(P|0)>0;if(Q){S=0;do{c[p+(S<<2)>>2]=c[r+(S*36|0)+28>>2];c[q+(S<<2)>>2]=c[r+(S*36|0)+32>>2];S=S+1|0;}while((S|0)<(P|0))}do{if((P|0)==2){T=+g[l>>2];U=+g[l+4>>2];V=+g[J>>2];W=+g[J+4>>2];X=V-T;Y=W-U;Z=T*X+U*Y;if(!(Z<-0.0)){g[K>>2]=1.0;c[s>>2]=1;_=13;break}U=V*X+W*Y;if(U>0.0){Y=1.0/(U-Z);g[K>>2]=U*Y;g[L>>2]=Y*(-0.0-Z);c[s>>2]=2;_=14;break}else{g[L>>2]=1.0;c[s>>2]=1;On(M|0,N|0,36)|0;_=13;break}}else if((P|0)==3){re(o);S=c[s>>2]|0;if((S|0)==3){_=11;break a}else{$=S;_=12}}else{$=P;_=12}}while(0);do{if((_|0)==12){_=0;if(($|0)==2){_=14;break}else if(($|0)==1){_=13;break}S=8600;Z=+g[S>>2];aa=Z;ba=+g[S+4>>2];ca=$}}while(0);do{if((_|0)==13){_=0;aa=-0.0- +g[F>>2];ba=-0.0- +g[G>>2];ca=1}else if((_|0)==14){_=0;Z=+g[F>>2];Y=+g[H>>2]-Z;U=+g[G>>2];W=+g[I>>2]-U;if(Y*(-0.0-U)-W*(-0.0-Z)>0.0){aa=W*-1.0;ba=Y;ca=2;break}else{aa=W;ba=Y*-1.0;ca=2;break}}}while(0);if(ba*ba+aa*aa<1.4210854715202004e-14){da=O;ea=ca;_=31;break}S=r+(ca*36|0)|0;Y=-0.0-ba;W=t*(-0.0-aa)+u*Y;Z=t*Y+aa*u;fa=c[v>>2]|0;ga=c[w>>2]|0;if((ga|0)>1){Y=Z*+g[fa+4>>2]+W*+g[fa>>2];ha=1;ia=0;while(1){U=W*+g[fa+(ha<<3)>>2]+Z*+g[fa+(ha<<3)+4>>2];ja=U>Y;ka=ja?ha:ia;la=ha+1|0;if((la|0)<(ga|0)){Y=ja?U:Y;ha=la;ia=ka}else{ma=ka;break}}}else{ma=0}c[r+(ca*36|0)+28>>2]=ma;Y=+g[fa+(ma<<3)>>2];Z=+g[fa+(ma<<3)+4>>2];W=x+(t*Y-u*Z);ia=S;U=+W;X=+(Y*u+t*Z+y);g[ia>>2]=U;g[ia+4>>2]=X;X=aa*z+ba*A;U=ba*z+aa*B;ia=c[j>>2]|0;ha=c[C>>2]|0;if((ha|0)>1){Z=U*+g[ia+4>>2]+X*+g[ia>>2];ga=1;ka=0;while(1){Y=X*+g[ia+(ga<<3)>>2]+U*+g[ia+(ga<<3)+4>>2];la=Y>Z;ja=la?ga:ka;na=ga+1|0;if((na|0)<(ha|0)){Z=la?Y:Z;ga=na;ka=ja}else{oa=ja;break}}}else{oa=0}c[r+(ca*36|0)+32>>2]=oa;Z=+g[ia+(oa<<3)>>2];U=+g[ia+(oa<<3)+4>>2];X=D+(z*Z-A*U);ka=r+(ca*36|0)+8|0;Y=+X;V=+(Z*A+z*U+E);g[ka>>2]=Y;g[ka+4>>2]=V;ka=r+(ca*36|0)+16|0;V=+(X-W);X=+(+g[r+(ca*36|0)+12>>2]- +g[r+(ca*36|0)+4>>2]);g[ka>>2]=V;g[ka+4>>2]=X;pa=O+1|0;c[2146]=(c[2146]|0)+1;if(Q){ka=0;do{if((ma|0)==(c[p+(ka<<2)>>2]|0)){if((oa|0)==(c[q+(ka<<2)>>2]|0)){_=30;break a}}ka=ka+1|0;}while((ka|0)<(P|0))}ka=(c[s>>2]|0)+1|0;c[s>>2]=ka;if((pa|0)<20){O=pa;P=ka}else{da=pa;ea=ka;_=31;break}}if((_|0)==11){P=c[2144]|0;c[2144]=(P|0)>(O|0)?P:O;qa=O;ra=d+8|0;_=35}else if((_|0)==30){da=pa;ea=c[s>>2]|0;_=31}do{if((_|0)==31){s=c[2144]|0;c[2144]=(s|0)>(da|0)?s:da;s=d+8|0;if((ea|0)==1){pa=d;O=c[n>>2]|0;P=c[n+4>>2]|0;c[pa>>2]=O;c[pa+4>>2]=P;pa=m+8|0;q=s;oa=c[pa>>2]|0;p=c[pa+4>>2]|0;c[q>>2]=oa;c[q+4>>2]=p;E=(c[k>>2]=O,+g[k>>2]);z=(c[k>>2]=oa,+g[k>>2]);A=(c[k>>2]=P,+g[k>>2]);sa=E;ta=z;ua=A;va=(c[k>>2]=p,+g[k>>2]);wa=1;xa=da;ya=s;break}else if((ea|0)==2){A=+g[K>>2];z=+g[L>>2];E=A*+g[m>>2]+z*+g[o+36>>2];D=A*+g[o+4>>2]+z*+g[m+40>>2];p=d;B=+E;aa=+D;g[p>>2]=B;g[p+4>>2]=aa;aa=A*+g[m+8>>2]+z*+g[o+44>>2];B=A*+g[o+12>>2]+z*+g[m+48>>2];p=s;z=+aa;A=+B;g[p>>2]=z;g[p+4>>2]=A;sa=E;ta=aa;ua=D;va=B;wa=2;xa=da;ya=s;break}else if((ea|0)==3){qa=da;ra=s;_=35;break}else{sa=+g[d>>2];ta=+g[s>>2];ua=+g[d+4>>2];va=+g[d+12>>2];wa=ea;xa=da;ya=s;break}}}while(0);if((_|0)==35){B=+g[K>>2];D=+g[L>>2];aa=+g[m+96>>2];E=B*+g[m>>2]+D*+g[o+36>>2]+aa*+g[m+72>>2];A=B*+g[o+4>>2]+D*+g[m+40>>2]+aa*+g[o+76>>2];L=d;K=(g[k>>2]=E,c[k>>2]|0);da=K|0;aa=+A;c[L>>2]=da;g[L+4>>2]=aa;L=ra;c[L>>2]=da;g[L+4>>2]=aa;sa=E;ta=E;ua=A;va=A;wa=3;xa=qa;ya=ra}ra=d|0;qa=ya|0;A=sa-ta;L=d+4|0;da=d+12|0;ta=ua-va;K=d+16|0;g[K>>2]=+R(A*A+ta*ta);c[d+20>>2]=xa;if((wa|0)==2){ta=+g[F>>2]- +g[H>>2];A=+g[G>>2]- +g[I>>2];za=+R(ta*ta+A*A);_=39}else if((wa|0)==3){A=+g[F>>2];ta=+g[G>>2];za=(+g[H>>2]-A)*(+g[o+92>>2]-ta)-(+g[I>>2]-ta)*(+g[m+88>>2]-A);_=39}else{g[e>>2]=0.0;b[e+4>>1]=wa;if((wa|0)>0){Aa=0;_=41}}if((_|0)==39){g[e>>2]=za;b[e+4>>1]=wa;Aa=0;_=41}if((_|0)==41){while(1){_=0;a[e+6+Aa|0]=c[r+(Aa*36|0)+28>>2];a[e+9+Aa|0]=c[r+(Aa*36|0)+32>>2];m=Aa+1|0;if((m|0)<(wa|0)){Aa=m;_=41}else{break}}}if((a[f+88|0]|0)==0){i=h;return}za=+g[f+24>>2];A=+g[f+52>>2];ta=+g[K>>2];va=za+A;if(!(ta>va&ta>1.1920928955078125e-7)){f=d;d=(g[k>>2]=(+g[ra>>2]+ +g[qa>>2])*.5,c[k>>2]|0);_=d|0;ua=+((+g[L>>2]+ +g[da>>2])*.5);c[f>>2]=_;g[f+4>>2]=ua;f=ya;c[f>>2]=_;g[f+4>>2]=ua;g[K>>2]=0.0;i=h;return}g[K>>2]=ta-va;va=+g[qa>>2];ta=+g[ra>>2];ua=va-ta;sa=+g[da>>2];E=+g[L>>2];aa=sa-E;D=+R(ua*ua+aa*aa);if(D<1.1920928955078125e-7){Ba=ua;Ca=aa}else{B=1.0/D;Ba=ua*B;Ca=aa*B}g[ra>>2]=za*Ba+ta;g[L>>2]=za*Ca+E;g[qa>>2]=va-A*Ba;g[da>>2]=sa-A*Ca;i=h;return}function te(a,e,f,h,i,j){a=a|0;e=e|0;f=f|0;h=h|0;i=i|0;j=j|0;var k=0,l=0,m=0,n=0,o=0,p=0,q=0,r=0,s=0,t=0,u=0,v=0,w=0,x=0,y=0,z=0,A=0,B=0.0,C=0.0,D=0.0,E=0.0,F=0.0,G=0.0,H=0.0,I=0.0,J=0.0,K=0.0,L=0;k=b[e+4>>1]|0;l=k&65535;m=a+108|0;c[m>>2]=l;n=a|0;if(k<<16>>16==0){o=l}else{l=f+16|0;k=i+16|0;p=h+12|0;q=h+8|0;r=h|0;s=h+4|0;t=j+12|0;u=j+8|0;v=j|0;w=j+4|0;x=0;while(1){y=d[e+6+x|0]|0;c[n+(x*36|0)+28>>2]=y;z=d[e+9+x|0]|0;c[n+(x*36|0)+32>>2]=z;A=(c[l>>2]|0)+(y<<3)|0;B=+g[A>>2];C=+g[A+4>>2];A=(c[k>>2]|0)+(z<<3)|0;D=+g[A>>2];E=+g[A+4>>2];F=+g[p>>2];G=+g[q>>2];H=+g[r>>2]+(B*F-C*G);A=n+(x*36|0)|0;I=+H;J=+(F*C+B*G+ +g[s>>2]);g[A>>2]=I;g[A+4>>2]=J;J=+g[t>>2];I=+g[u>>2];G=+g[v>>2]+(D*J-E*I);A=n+(x*36|0)+8|0;B=+G;C=+(E*J+D*I+ +g[w>>2]);g[A>>2]=B;g[A+4>>2]=C;A=n+(x*36|0)+16|0;C=+(G-H);H=+(+g[n+(x*36|0)+12>>2]- +g[n+(x*36|0)+4>>2]);g[A>>2]=C;g[A+4>>2]=H;g[n+(x*36|0)+24>>2]=0.0;A=x+1|0;z=c[m>>2]|0;if((A|0)<(z|0)){x=A}else{o=z;break}}}do{if((o|0)>1){H=+g[e>>2];if((o|0)==3){C=+g[a+16>>2];G=+g[a+20>>2];K=(+g[a+52>>2]-C)*(+g[a+92>>2]-G)-(+g[a+56>>2]-G)*(+g[a+88>>2]-C)}else if((o|0)==2){C=+g[a+16>>2]- +g[a+52>>2];G=+g[a+20>>2]- +g[a+56>>2];K=+R(C*C+G*G)}else{K=0.0}if(!(K<H*.5)){if(!(H*2.0<K|K<1.1920928955078125e-7)){L=11;break}}c[m>>2]=0}else{L=11}}while(0);do{if((L|0)==11){if((o|0)==0){break}return}}while(0);c[a+28>>2]=0;c[a+32>>2]=0;o=c[f+16>>2]|0;K=+g[o>>2];H=+g[o+4>>2];o=c[i+16>>2]|0;G=+g[o>>2];C=+g[o+4>>2];B=+g[h+12>>2];I=+g[h+8>>2];D=+g[h>>2]+(K*B-H*I);J=B*H+K*I+ +g[h+4>>2];h=a;I=+D;K=+J;g[h>>2]=I;g[h+4>>2]=K;K=+g[j+12>>2];I=+g[j+8>>2];H=+g[j>>2]+(G*K-C*I);B=C*K+G*I+ +g[j+4>>2];j=a+8|0;I=+H;G=+B;g[j>>2]=I;g[j+4>>2]=G;j=a+16|0;G=+(H-D);D=+(B-J);g[j>>2]=G;g[j+4>>2]=D;g[a+24>>2]=1.0;c[m>>2]=1;return}function ue(b,d){b=b|0;d=d|0;var e=0,f=0,h=0;e=Vm(d,48)|0;if((e|0)==0){f=0}else{c[e>>2]=5976;c[e+4>>2]=1;g[e+8>>2]=.009999999776482582;Nn(e+28|0,0,18)|0;f=e}c[f+4>>2]=c[b+4>>2];g[f+8>>2]=+g[b+8>>2];e=b+12|0;d=f+12|0;h=c[e+4>>2]|0;c[d>>2]=c[e>>2];c[d+4>>2]=h;h=b+20|0;d=f+20|0;e=c[h+4>>2]|0;c[d>>2]=c[h>>2];c[d+4>>2]=e;e=b+28|0;d=f+28|0;h=c[e+4>>2]|0;c[d>>2]=c[e>>2];c[d+4>>2]=h;h=b+36|0;d=f+36|0;e=c[h+4>>2]|0;c[d>>2]=c[h>>2];c[d+4>>2]=e;a[f+44|0]=a[b+44|0]|0;a[f+45|0]=a[b+45|0]|0;return f|0}function ve(a){a=a|0;return 1}function we(a,b,c){a=a|0;b=b|0;c=c|0;return 0}function xe(a,b,c,d,e,f){a=a|0;b=b|0;c=c|0;d=d|0;e=e|0;f=f|0;var h=0.0,i=0.0,j=0.0,k=0.0,l=0.0,m=0.0,n=0.0,o=0.0,p=0.0,q=0.0,r=0.0,s=0.0,t=0.0,u=0.0,v=0.0;h=+g[b+12>>2];i=+g[a+12>>2];j=+g[b+8>>2];k=+g[a+16>>2];l=+g[b>>2];m=l+(h*i-j*k);n=+g[b+4>>2];o=i*j+h*k+n;k=+g[a+20>>2];i=+g[a+24>>2];p=l+(h*k-j*i);l=n+(j*k+h*i);i=+g[c>>2];h=i-m;k=+g[c+4>>2];j=k-o;n=p-m;m=l-o;o=h*n+j*m;do{if(o>0.0){q=n*n+m*m;if(o>q){r=i-p;s=k-l;break}else{t=o/q;r=h-n*t;s=j-m*t;break}}else{r=h;s=j}}while(0);j=+R(s*s+r*r);g[d>>2]=j;if(j>0.0){h=1.0/j;u=r*h;v=s*h}else{d=8600;h=+g[d>>2];u=h;v=+g[d+4>>2]}d=e;h=+u;u=+v;g[d>>2]=h;g[d+4>>2]=u;return}function ye(a,b,c,d,e){a=a|0;b=b|0;c=c|0;d=d|0;e=e|0;var f=0.0,h=0.0,i=0.0,j=0.0,k=0.0,l=0,m=0.0,n=0.0,o=0.0,p=0.0,q=0.0,r=0.0,s=0.0,t=0.0,u=0.0,v=0;f=+g[d>>2];h=+g[c>>2]-f;i=+g[d+4>>2];j=+g[c+4>>2]-i;e=d+12|0;k=+g[e>>2];l=d+8|0;m=+g[l>>2];n=h*k+j*m;o=-0.0-m;p=k*j+h*o;h=+g[c+8>>2]-f;f=+g[c+12>>2]-i;i=k*h+m*f-n;m=h*o+k*f-p;d=a+12|0;f=+g[d>>2];k=+g[d+4>>2];d=a+20|0;o=+g[d>>2];h=o-f;o=+g[d+4>>2]-k;j=-0.0-h;q=h*h+o*o;r=+R(q);if(r<1.1920928955078125e-7){s=o;t=j}else{u=1.0/r;s=o*u;t=u*j}j=(k-p)*t+(f-n)*s;u=m*t+i*s;if(u==0.0){v=0;return v|0}r=j/u;if(r<0.0){v=0;return v|0}if(+g[c+16>>2]<r|q==0.0){v=0;return v|0}u=(h*(n+i*r-f)+o*(p+m*r-k))/q;if(u<0.0|u>1.0){v=0;return v|0}g[b+8>>2]=r;r=+g[e>>2];u=+g[l>>2];q=s*r-t*u;k=t*r+s*u;if(j>0.0){l=b;j=+(-0.0-q);u=+(-0.0-k);g[l>>2]=j;g[l+4>>2]=u;v=1;return v|0}else{l=b;u=+q;q=+k;g[l>>2]=u;g[l+4>>2]=q;v=1;return v|0}return 0}function ze(a,b,c,d){a=a|0;b=b|0;c=c|0;d=d|0;var e=0.0,f=0.0,h=0.0,i=0.0,j=0.0,k=0.0,l=0.0,m=0.0,n=0.0;e=+g[c+12>>2];f=+g[a+12>>2];h=+g[c+8>>2];i=+g[a+16>>2];j=+g[c>>2];k=j+(e*f-h*i);l=+g[c+4>>2];m=f*h+e*i+l;i=+g[a+20>>2];f=+g[a+24>>2];n=j+(e*i-h*f);j=l+(h*i+e*f);f=+g[a+8>>2];a=b;e=+((k<n?k:n)-f);i=+((m<j?m:j)-f);g[a>>2]=e;g[a+4>>2]=i;a=b+8|0;i=+(f+(k>n?k:n));n=+(f+(m>j?m:j));g[a>>2]=i;g[a+4>>2]=n;return}function Ae(a,b,c){a=a|0;b=b|0;c=+c;var d=0,e=0.0;g[b>>2]=0.0;d=b+4|0;c=+((+g[a+12>>2]+ +g[a+20>>2])*.5);e=+((+g[a+16>>2]+ +g[a+24>>2])*.5);g[d>>2]=c;g[d+4>>2]=e;g[b+12>>2]=0.0;return}function Be(a,b){a=a|0;b=b|0;var d=0,e=0,f=0;d=Vm(b,152)|0;if((d|0)==0){e=0}else{c[d>>2]=5360;c[d+4>>2]=2;g[d+8>>2]=.009999999776482582;c[d+148>>2]=0;g[d+12>>2]=0.0;g[d+16>>2]=0.0;e=d}c[e+4>>2]=c[a+4>>2];g[e+8>>2]=+g[a+8>>2];d=a+12|0;b=e+12|0;f=c[d+4>>2]|0;c[b>>2]=c[d>>2];c[b+4>>2]=f;On(e+20|0,a+20|0,64)|0;On(e+84|0,a+84|0,64)|0;c[e+148>>2]=c[a+148>>2];return e|0}function Ce(a){a=a|0;return 1}function De(a,b,d){a=a|0;b=b|0;d=d|0;var e=0,f=0,h=0,j=0,l=0,m=0,n=0,o=0,p=0.0,q=0.0,r=0,s=0,t=0.0,u=0.0,v=0,w=0.0,x=0,y=0,z=0,A=0,B=0.0,C=0.0,D=0,E=0,F=0,G=0.0,H=0.0,I=0.0,J=0.0,K=0.0,L=0.0,M=0,N=0,O=0.0;e=i;i=i+96|0;f=e|0;h=e+64|0;if((d|0)<3){c[a+148>>2]=4;g[a+20>>2]=-1.0;g[a+24>>2]=-1.0;g[a+28>>2]=1.0;g[a+32>>2]=-1.0;g[a+36>>2]=1.0;g[a+40>>2]=1.0;g[a+44>>2]=-1.0;g[a+48>>2]=1.0;g[a+84>>2]=0.0;g[a+88>>2]=-1.0;g[a+92>>2]=1.0;g[a+96>>2]=0.0;g[a+100>>2]=0.0;g[a+104>>2]=1.0;g[a+108>>2]=-1.0;g[a+112>>2]=0.0;g[a+12>>2]=0.0;g[a+16>>2]=0.0;i=e;return}j=(d|0)<8?d:8;do{if((j|0)>0){d=0;l=0;while(1){m=b+(d<<3)|0;n=c[m>>2]|0;o=c[m+4>>2]|0;p=(c[k>>2]=n,+g[k>>2]);m=o;q=(c[k>>2]=m,+g[k>>2]);r=0;while(1){if((r|0)>=(l|0)){s=7;break}t=p- +g[f+(r<<3)>>2];u=q- +g[f+(r<<3)+4>>2];if(t*t+u*u<.0024999999441206455){v=l;break}else{r=r+1|0}}if((s|0)==7){s=0;r=f+(l<<3)|0;c[r>>2]=n|0;c[r+4>>2]=m|o&0;v=l+1|0}r=d+1|0;if((r|0)<(j|0)){d=r;l=v}else{break}}if((v|0)<3){break}l=0;q=+g[f>>2];d=1;while(1){p=+g[f+(d<<3)>>2];do{if(p>q){s=22}else{if(!(p==q)){w=q;x=l;break}if(+g[f+(d<<3)+4>>2]<+g[f+(l<<3)+4>>2]){s=22}else{w=q;x=l}}}while(0);if((s|0)==22){s=0;w=p;x=d}o=d+1|0;if((o|0)<(v|0)){l=x;q=w;d=o}else{y=x;z=0;break}}while(1){c[h+(z<<2)>>2]=y;d=0;l=1;while(1){do{if((d|0)==(y|0)){A=l}else{q=+g[f+(y<<3)>>2];u=+g[f+(d<<3)>>2]-q;t=+g[f+(y<<3)+4>>2];B=+g[f+(d<<3)+4>>2]-t;C=+g[f+(l<<3)>>2]-q;q=+g[f+(l<<3)+4>>2]-t;t=u*q-B*C;o=t<0.0?l:d;if(!(t==0.0)){A=o;break}if(!(C*C+q*q>u*u+B*B)){A=o;break}A=l}}while(0);o=l+1|0;if((o|0)<(v|0)){d=A;l=o}else{break}}D=z+1|0;if((A|0)==(x|0)){break}else{y=A;z=D}}c[a+148>>2]=D;l=(D|0)>0;do{if(l){d=0;do{o=f+(c[h+(d<<2)>>2]<<3)|0;m=a+20+(d<<3)|0;n=c[o+4>>2]|0;c[m>>2]=c[o>>2];c[m+4>>2]=n;d=d+1|0;}while((d|0)<(D|0));if(l){E=0}else{s=30;break}while(1){d=E+1|0;n=(d|0)<(D|0);m=n?d:0;p=+g[a+20+(m<<3)+4>>2]- +g[a+20+(E<<3)+4>>2];o=a+84+(E<<3)|0;r=o;B=+p;u=+((+g[a+20+(m<<3)>>2]- +g[a+20+(E<<3)>>2])*-1.0);g[r>>2]=B;g[r+4>>2]=u;r=a+84+(E<<3)+4|0;u=+g[r>>2];B=+R(p*p+u*u);if(!(B<1.1920928955078125e-7)){q=1.0/B;g[o>>2]=p*q;g[r>>2]=u*q}if(n){E=d}else{break}}d=a+12|0;n=a+20|0;if(l){F=0;G=0.0;H=0.0;I=0.0}else{J=0.0;K=0.0;L=0.0;M=d;break}while(1){r=a+20+(F<<3)|0;q=+g[r>>2];u=+g[r+4>>2];r=F+1|0;o=(r|0)<(D|0);if(o){N=a+20+(r<<3)|0}else{N=n}m=N;p=+g[m>>2];B=+g[m+4>>2];C=(q*B-u*p)*.5;t=G+C;O=C*.3333333432674408;C=H+(q+0.0+p)*O;p=I+(u+0.0+B)*O;if(o){F=r;G=t;H=C;I=p}else{J=t;K=C;L=p;M=d;break}}}else{s=30}}while(0);if((s|0)==30){J=0.0;K=0.0;L=0.0;M=a+12|0}p=1.0/J;l=M;C=+(K*p);t=+(L*p);g[l>>2]=C;g[l+4>>2]=t;i=e;return}}while(0);c[a+148>>2]=4;g[a+20>>2]=-1.0;g[a+24>>2]=-1.0;g[a+28>>2]=1.0;g[a+32>>2]=-1.0;g[a+36>>2]=1.0;g[a+40>>2]=1.0;g[a+44>>2]=-1.0;g[a+48>>2]=1.0;g[a+84>>2]=0.0;g[a+88>>2]=-1.0;g[a+92>>2]=1.0;g[a+96>>2]=0.0;g[a+100>>2]=0.0;g[a+104>>2]=1.0;g[a+108>>2]=-1.0;g[a+112>>2]=0.0;g[a+12>>2]=0.0;g[a+16>>2]=0.0;i=e;return}function Ee(a,b,d){a=a|0;b=b|0;d=d|0;var e=0.0,f=0.0,h=0.0,i=0.0,j=0.0,k=0.0,l=0,m=0;e=+g[d>>2]- +g[b>>2];f=+g[d+4>>2]- +g[b+4>>2];h=+g[b+12>>2];i=+g[b+8>>2];j=e*h+f*i;k=h*f+e*(-0.0-i);b=c[a+148>>2]|0;d=0;while(1){if((d|0)>=(b|0)){l=1;m=4;break}if((j- +g[a+20+(d<<3)>>2])*+g[a+84+(d<<3)>>2]+(k- +g[a+20+(d<<3)+4>>2])*+g[a+84+(d<<3)+4>>2]>0.0){l=0;m=4;break}else{d=d+1|0}}if((m|0)==4){return l|0}return 0}function Fe(a,b,d,e,f,h){a=a|0;b=b|0;d=d|0;e=e|0;f=f|0;h=h|0;var i=0.0,j=0.0,k=0.0,l=0.0,m=0.0,n=0.0,o=0,p=0,q=0,r=0,s=0.0,t=0.0,u=0.0,v=0.0,w=0.0,x=0.0,y=0.0,z=0.0,A=0.0,B=0.0,C=0.0,D=0.0,E=0.0;i=+g[d>>2]- +g[b>>2];j=+g[d+4>>2]- +g[b+4>>2];d=b+12|0;k=+g[d>>2];h=b+8|0;l=+g[h>>2];m=i*k+j*l;n=k*j+i*(-0.0-l);b=c[a+148>>2]|0;o=(b|0)>0;do{if(o){l=n;i=m;p=0;j=-3.4028234663852886e+38;while(1){q=a+84+(p<<3)|0;k=(m- +g[a+20+(p<<3)>>2])*+g[q>>2]+(n- +g[a+20+(p<<3)+4>>2])*+g[a+84+(p<<3)+4>>2];if(k>j){r=q;s=+g[r>>2];t=k;u=s;v=+g[r+4>>2]}else{t=j;u=i;v=l}r=p+1|0;if((r|0)<(b|0)){l=v;i=u;p=r;j=t}else{break}}if(!(t>0.0)){w=t;x=u;y=v;break}j=t*t;if(o){i=v;l=u;s=j;p=0;while(1){k=m- +g[a+20+(p<<3)>>2];z=n- +g[a+20+(p<<3)+4>>2];A=k*k+z*z;r=s>A;B=r?z:i;z=r?k:l;k=r?A:s;r=p+1|0;if((r|0)<(b|0)){i=B;l=z;s=k;p=r}else{C=B;D=z;E=k;break}}}else{C=v;D=u;E=j}g[e>>2]=+R(E);s=+g[d>>2];l=+g[h>>2];i=D*s-C*l;k=C*s+D*l;p=f;l=+i;s=+k;g[p>>2]=l;g[p+4>>2]=s;s=+R(i*i+k*k);if(s<1.1920928955078125e-7){return}l=1.0/s;g[f>>2]=i*l;g[f+4>>2]=k*l;return}else{w=-3.4028234663852886e+38;x=m;y=n}}while(0);g[e>>2]=w;w=+g[d>>2];n=+g[h>>2];h=f;m=+(x*w-y*n);D=+(y*w+x*n);g[h>>2]=m;g[h+4>>2]=D;return}function Ge(a,b,d,e,f){a=a|0;b=b|0;d=d|0;e=e|0;f=f|0;var h=0.0,i=0.0,j=0.0,k=0.0,l=0.0,m=0,n=0.0,o=0.0,p=0.0,q=0.0,r=0,s=0,t=0,u=0.0,v=0,w=0.0,x=0,y=0.0;h=+g[e>>2];i=+g[d>>2]-h;j=+g[e+4>>2];k=+g[d+4>>2]-j;f=e+12|0;l=+g[f>>2];m=e+8|0;n=+g[m>>2];o=i*l+k*n;p=-0.0-n;q=l*k+i*p;i=+g[d+8>>2]-h;h=+g[d+12>>2]-j;j=l*i+n*h-o;n=i*p+l*h-q;e=c[a+148>>2]|0;h=0.0;r=0;s=-1;l=+g[d+16>>2];a:while(1){if((r|0)>=(e|0)){t=12;break}p=+g[a+84+(r<<3)>>2];i=+g[a+84+(r<<3)+4>>2];k=(+g[a+20+(r<<3)>>2]-o)*p+(+g[a+20+(r<<3)+4>>2]-q)*i;u=j*p+n*i;b:do{if(u==0.0){if(k<0.0){v=0;t=14;break a}else{w=h;x=s;y=l}}else{do{if(u<0.0){if(!(k<h*u)){break}w=k/u;x=r;y=l;break b}}while(0);if(!(u>0.0)){w=h;x=s;y=l;break}if(!(k<l*u)){w=h;x=s;y=l;break}w=h;x=s;y=k/u}}while(0);if(y<w){v=0;t=14;break}else{h=w;r=r+1|0;s=x;l=y}}if((t|0)==12){if(!((s|0)>-1)){v=0;return v|0}g[b+8>>2]=h;h=+g[f>>2];y=+g[a+84+(s<<3)>>2];l=+g[m>>2];w=+g[a+84+(s<<3)+4>>2];s=b;n=+(h*y-l*w);j=+(y*l+h*w);g[s>>2]=n;g[s+4>>2]=j;v=1;return v|0}else if((t|0)==14){return v|0}return 0}function He(a,b,d,e){a=a|0;b=b|0;d=d|0;e=e|0;var f=0.0,h=0.0,i=0.0,j=0.0,k=0.0,l=0.0,m=0.0,n=0.0,o=0.0,p=0.0,q=0.0,r=0.0,s=0.0,t=0.0,u=0.0,v=0,w=0.0,x=0.0,y=0.0,z=0.0;f=+g[d+12>>2];h=+g[a+20>>2];i=+g[d+8>>2];j=+g[a+24>>2];k=+g[d>>2];l=k+(f*h-i*j);m=+g[d+4>>2];n=h*i+f*j+m;d=c[a+148>>2]|0;if((d|0)>1){j=n;h=l;o=n;p=l;e=1;while(1){q=+g[a+20+(e<<3)>>2];r=+g[a+20+(e<<3)+4>>2];s=k+(f*q-i*r);t=q*i+f*r+m;r=h<s?h:s;q=j<t?j:t;u=p>s?p:s;s=o>t?o:t;v=e+1|0;if((v|0)<(d|0)){j=q;h=r;o=s;p=u;e=v}else{w=q;x=r;y=s;z=u;break}}}else{w=n;x=l;y=n;z=l}l=+g[a+8>>2];a=b;n=+(x-l);x=+(w-l);g[a>>2]=n;g[a+4>>2]=x;a=b+8|0;x=+(z+l);z=+(y+l);g[a>>2]=x;g[a+4>>2]=z;return}function Ie(a,b,d){a=a|0;b=b|0;d=+d;var e=0,f=0,h=0.0,i=0.0,j=0,k=0.0,l=0.0,m=0.0,n=0.0,o=0.0,p=0.0,q=0.0,r=0.0,s=0.0,t=0,u=0.0,v=0,w=0.0,x=0.0,y=0.0,z=0.0,A=0,B=0,C=0,D=0,E=0.0,F=0.0,G=0.0,H=0.0,I=0.0,J=0.0,K=0.0;e=c[a+148>>2]|0;f=(e|0)>0;do{if(f){h=0.0;i=0.0;j=0;do{i=i+ +g[a+20+(j<<3)>>2];h=h+ +g[a+20+(j<<3)+4>>2];j=j+1|0;}while((j|0)<(e|0));k=1.0/+(e|0);l=i*k;m=h*k;if(!f){n=0.0;o=0.0;p=0.0;q=0.0;r=l;s=m;break}j=a+20|0;t=a+24|0;k=0.0;u=0.0;v=0;w=0.0;x=0.0;while(1){y=+g[a+20+(v<<3)>>2]-l;z=+g[a+20+(v<<3)+4>>2]-m;A=v+1|0;B=(A|0)<(e|0);if(B){C=a+20+(A<<3)|0;D=a+20+(A<<3)+4|0}else{C=j;D=t}E=+g[C>>2]-l;F=+g[D>>2]-m;G=y*F-z*E;H=G*.5;I=x+H;J=H*.3333333432674408;H=u+(y+E)*J;K=k+(z+F)*J;J=w+G*.0833333358168602*(E*E+(y*y+y*E)+(F*F+(z*z+z*F)));if(B){k=K;u=H;v=A;w=J;x=I}else{n=K;o=H;p=J;q=I;r=l;s=m;break}}}else{m=1.0/+(e|0);n=0.0;o=0.0;p=0.0;q=0.0;r=m*0.0;s=m*0.0}}while(0);m=q*d;g[b>>2]=m;l=1.0/q;q=o*l;o=n*l;l=r+q;r=s+o;e=b+4|0;s=+l;n=+r;g[e>>2]=s;g[e+4>>2]=n;g[b+12>>2]=p*d+m*(l*l+r*r-(q*q+o*o));return}function Je(a,b){a=a|0;b=b|0;var d=0,e=0;d=Vm(b,20)|0;if((d|0)==0){e=0}else{c[d>>2]=5480;Nn(d+4|0,0,16)|0;e=d}c[e+4>>2]=c[a+4>>2];g[e+8>>2]=+g[a+8>>2];d=a+12|0;a=e+12|0;b=c[d+4>>2]|0;c[a>>2]=c[d>>2];c[a+4>>2]=b;return e|0}function Ke(a){a=a|0;return 1}function Le(a,b,c){a=a|0;b=b|0;c=c|0;var d=0.0,e=0.0,f=0.0,h=0.0,i=0.0,j=0.0;d=+g[b+12>>2];e=+g[a+12>>2];f=+g[b+8>>2];h=+g[a+16>>2];i=+g[c>>2]-(+g[b>>2]+(d*e-f*h));j=+g[c+4>>2]-(+g[b+4>>2]+(e*f+d*h));h=+g[a+8>>2];return i*i+j*j<=h*h|0}function Me(a,b,c,d,e,f){a=a|0;b=b|0;c=c|0;d=d|0;e=e|0;f=f|0;var h=0.0,i=0.0,j=0.0,k=0.0,l=0.0,m=0.0;h=+g[b+12>>2];i=+g[a+12>>2];j=+g[b+8>>2];k=+g[a+16>>2];l=+g[c>>2]-(+g[b>>2]+(h*i-j*k));m=+g[c+4>>2]-(+g[b+4>>2]+(i*j+h*k));k=+R(l*l+m*m);g[d>>2]=k- +g[a+8>>2];h=1.0/k;a=e;k=+(l*h);l=+(m*h);g[a>>2]=k;g[a+4>>2]=l;return}function Ne(a,b,c,d,e){a=a|0;b=b|0;c=c|0;d=d|0;e=e|0;var f=0.0,h=0.0,i=0.0,j=0.0,k=0.0,l=0.0,m=0.0,n=0.0,o=0;f=+g[d+12>>2];h=+g[a+12>>2];i=+g[d+8>>2];j=+g[a+16>>2];k=+g[c>>2];l=k-(+g[d>>2]+(f*h-i*j));m=+g[c+4>>2];n=m-(+g[d+4>>2]+(h*i+f*j));j=+g[a+8>>2];f=+g[c+8>>2]-k;k=+g[c+12>>2]-m;m=l*f+n*k;i=f*f+k*k;h=m*m-(l*l+n*n-j*j)*i;if(h<0.0|i<1.1920928955078125e-7){o=0;return o|0}j=m+ +R(h);h=-0.0-j;if(j>-0.0){o=0;return o|0}if(i*+g[c+16>>2]<h){o=0;return o|0}j=h/i;g[b+8>>2]=j;i=l+f*j;f=n+k*j;c=b;j=+i;k=+f;g[c>>2]=j;g[c+4>>2]=k;k=+R(i*i+f*f);if(k<1.1920928955078125e-7){o=1;return o|0}j=1.0/k;g[b>>2]=i*j;g[b+4>>2]=f*j;o=1;return o|0}function Oe(a,b,c,d){a=a|0;b=b|0;c=c|0;d=d|0;var e=0.0,f=0.0,h=0.0,i=0.0,j=0.0,k=0.0;e=+g[c+12>>2];f=+g[a+12>>2];h=+g[c+8>>2];i=+g[a+16>>2];j=+g[c>>2]+(e*f-h*i);k=+g[c+4>>2]+(f*h+e*i);c=a+8|0;i=+g[c>>2];g[b>>2]=j-i;g[b+4>>2]=k-i;i=+g[c>>2];g[b+8>>2]=j+i;g[b+12>>2]=k+i;return}function Pe(a,b,d){a=a|0;b=b|0;d=+d;var e=0,f=0.0,h=0.0,i=0,j=0,k=0,l=0,m=0.0;e=a+8|0;f=+g[e>>2];h=f*d*3.1415927410125732*f;g[b>>2]=h;i=a+12|0;j=i;k=b+4|0;l=c[j+4>>2]|0;c[k>>2]=c[j>>2];c[k+4>>2]=l;f=+g[e>>2];d=+g[i>>2];m=+g[a+16>>2];g[b+12>>2]=h*(f*f*.5+(d*d+m*m));return}function Qe(a){a=a|0;c[a>>2]=5728;fn(c[a+12>>2]|0);Jn(a);return}function Re(a){a=a|0;var b=0;c[a>>2]=5728;b=a+12|0;fn(c[b>>2]|0);c[b>>2]=0;c[a+16>>2]=0;return}function Se(a,b,d){a=a|0;b=b|0;d=d|0;var e=0,f=0;e=a+16|0;c[e>>2]=d;f=en(d<<3)|0;c[a+12>>2]=f;On(f|0,b|0,c[e>>2]<<3)|0;Nn(a+20|0,0,18)|0;return}function Te(b,d){b=b|0;d=d|0;var e=0,f=0,h=0,i=0;e=Vm(d,40)|0;if((e|0)==0){f=0}else{c[e>>2]=5728;c[e+4>>2]=3;g[e+8>>2]=.009999999776482582;c[e+12>>2]=0;c[e+16>>2]=0;a[e+36|0]=0;a[e+37|0]=0;f=e}e=c[b+12>>2]|0;d=c[b+16>>2]|0;h=f+16|0;c[h>>2]=d;i=en(d<<3)|0;c[f+12>>2]=i;On(i|0,e|0,c[h>>2]<<3)|0;Nn(f+20|0,0,18)|0;h=b+20|0;e=f+20|0;i=c[h+4>>2]|0;c[e>>2]=c[h>>2];c[e+4>>2]=i;i=b+28|0;e=f+28|0;h=c[i+4>>2]|0;c[e>>2]=c[i>>2];c[e+4>>2]=h;a[f+36|0]=a[b+36|0]|0;a[f+37|0]=a[b+37|0]|0;return f|0}function Ue(a){a=a|0;return(c[a+16>>2]|0)-1|0}function Ve(b,d,e){b=b|0;d=d|0;e=e|0;var f=0,h=0,i=0,j=0,k=0;c[d+4>>2]=1;g[d+8>>2]=+g[b+8>>2];f=b+12|0;h=(c[f>>2]|0)+(e<<3)|0;i=d+12|0;j=c[h+4>>2]|0;c[i>>2]=c[h>>2];c[i+4>>2]=j;j=(c[f>>2]|0)+(e+1<<3)|0;i=d+20|0;h=c[j+4>>2]|0;c[i>>2]=c[j>>2];c[i+4>>2]=h;h=d+28|0;if((e|0)>0){i=(c[f>>2]|0)+(e-1<<3)|0;j=h;k=c[i+4>>2]|0;c[j>>2]=c[i>>2];c[j+4>>2]=k;a[d+44|0]=1}else{k=b+20|0;j=h;h=c[k+4>>2]|0;c[j>>2]=c[k>>2];c[j+4>>2]=h;a[d+44|0]=a[b+36|0]|0}h=d+36|0;if(((c[b+16>>2]|0)-2|0)>(e|0)){j=(c[f>>2]|0)+(e+2<<3)|0;e=h;f=c[j+4>>2]|0;c[e>>2]=c[j>>2];c[e+4>>2]=f;a[d+45|0]=1;return}else{f=b+28|0;e=h;h=c[f+4>>2]|0;c[e>>2]=c[f>>2];c[e+4>>2]=h;a[d+45|0]=a[b+37|0]|0;return}}function We(b,d,e,f,h,j){b=b|0;d=d|0;e=e|0;f=f|0;h=h|0;j=j|0;var k=0,l=0,m=0,n=0,o=0,p=0,q=0;k=i;i=i+48|0;l=k|0;c[l>>2]=5976;Nn(l+28|0,0,18)|0;c[l+4>>2]=1;g[l+8>>2]=+g[b+8>>2];m=c[b+12>>2]|0;n=m+(j<<3)|0;o=l+12|0;p=c[n+4>>2]|0;c[o>>2]=c[n>>2];c[o+4>>2]=p;p=m+(j+1<<3)|0;o=l+20|0;n=c[p+4>>2]|0;c[o>>2]=c[p>>2];c[o+4>>2]=n;n=l+28|0;if((j|0)>0){o=m+(j-1<<3)|0;p=n;q=c[o+4>>2]|0;c[p>>2]=c[o>>2];c[p+4>>2]=q;a[l+44|0]=1}else{q=b+20|0;p=n;n=c[q+4>>2]|0;c[p>>2]=c[q>>2];c[p+4>>2]=n;a[l+44|0]=a[b+36|0]|0}n=l+36|0;if(((c[b+16>>2]|0)-2|0)>(j|0)){p=m+(j+2<<3)|0;j=n;m=c[p+4>>2]|0;c[j>>2]=c[p>>2];c[j+4>>2]=m;a[l+45|0]=1;xe(l,d,e,f,h,0);i=k;return}else{m=b+28|0;j=n;n=c[m+4>>2]|0;c[j>>2]=c[m>>2];c[j+4>>2]=n;a[l+45|0]=a[b+37|0]|0;xe(l,d,e,f,h,0);i=k;return}}function Xe(a,b,c){a=a|0;b=b|0;c=c|0;return 0}function Ye(a,b,d,e,f){a=a|0;b=b|0;d=d|0;e=e|0;f=f|0;var h=0,j=0,k=0,l=0,m=0;h=i;i=i+48|0;j=h|0;c[j>>2]=5976;c[j+4>>2]=1;g[j+8>>2]=.009999999776482582;Nn(j+28|0,0,18)|0;k=f+1|0;l=(k|0)==(c[a+16>>2]|0)?0:k;k=c[a+12>>2]|0;a=k+(f<<3)|0;f=j+12|0;m=c[a+4>>2]|0;c[f>>2]=c[a>>2];c[f+4>>2]=m;m=k+(l<<3)|0;l=j+20|0;k=c[m+4>>2]|0;c[l>>2]=c[m>>2];c[l+4>>2]=k;k=ye(j,b,d,e,0)|0;i=h;return k|0}function Ze(a,b,d,e){a=a|0;b=b|0;d=d|0;e=e|0;var f=0,h=0,i=0.0,j=0.0,k=0.0,l=0.0,m=0.0,n=0.0,o=0.0,p=0.0,q=0.0;f=e+1|0;h=(f|0)==(c[a+16>>2]|0)?0:f;f=c[a+12>>2]|0;i=+g[d+12>>2];j=+g[f+(e<<3)>>2];k=+g[d+8>>2];l=+g[f+(e<<3)+4>>2];m=+g[d>>2];n=m+(i*j-k*l);o=+g[d+4>>2];p=j*k+i*l+o;l=+g[f+(h<<3)>>2];j=+g[f+(h<<3)+4>>2];q=m+(i*l-k*j);m=o+(k*l+i*j);h=b;j=+(n<q?n:q);i=+(p<m?p:m);g[h>>2]=j;g[h+4>>2]=i;h=b+8|0;i=+(n>q?n:q);q=+(p>m?p:m);g[h>>2]=i;g[h+4>>2]=q;return}function _e(a,b,c){a=a|0;b=b|0;c=+c;Nn(b|0,0,16)|0;return}function $e(a){a=a|0;jf(a|0);c[a+28>>2]=0;c[a+48>>2]=16;c[a+52>>2]=0;c[a+44>>2]=en(128)|0;c[a+36>>2]=16;c[a+40>>2]=0;c[a+32>>2]=en(64)|0;return}function af(a){a=a|0;fn(c[a+32>>2]|0);fn(c[a+44>>2]|0);kf(a|0);return}function bf(a,b,d){a=a|0;b=b|0;d=d|0;var e=0,f=0,g=0,h=0,i=0;e=mf(a|0,b,d)|0;d=a+28|0;c[d>>2]=(c[d>>2]|0)+1;d=a+40|0;b=c[d>>2]|0;f=a+36|0;g=a+32|0;if((b|0)==(c[f>>2]|0)){a=c[g>>2]|0;c[f>>2]=b<<1;f=en(b<<3)|0;c[g>>2]=f;h=a;On(f|0,h|0,c[d>>2]<<2)|0;fn(h);i=c[d>>2]|0}else{i=b}c[(c[g>>2]|0)+(i<<2)>>2]=e;c[d>>2]=(c[d>>2]|0)+1;return e|0}function cf(a,b){a=a|0;b=b|0;var d=0,e=0,f=0,g=0,h=0,i=0;d=a+40|0;e=c[d>>2]|0;if((e|0)>0){f=c[a+32>>2]|0;g=0;h=e;while(1){e=f+(g<<2)|0;if((c[e>>2]|0)==(b|0)){c[e>>2]=-1;i=c[d>>2]|0}else{i=h}e=g+1|0;if((e|0)<(i|0)){g=e;h=i}else{break}}}i=a+28|0;c[i>>2]=(c[i>>2]|0)-1;of(a|0,b);return}function df(a,b,d,e){a=a|0;b=b|0;d=d|0;e=e|0;var f=0,g=0,h=0,i=0;if(!(qf(a|0,b,d,e)|0)){return}e=a+40|0;d=c[e>>2]|0;f=a+36|0;g=a+32|0;if((d|0)==(c[f>>2]|0)){a=c[g>>2]|0;c[f>>2]=d<<1;f=en(d<<3)|0;c[g>>2]=f;h=a;On(f|0,h|0,c[e>>2]<<2)|0;fn(h);i=c[e>>2]|0}else{i=d}c[(c[g>>2]|0)+(i<<2)>>2]=b;c[e>>2]=(c[e>>2]|0)+1;return}function ef(a,b){a=a|0;b=b|0;var d=0,e=0,f=0,g=0,h=0,i=0;d=a+40|0;e=c[d>>2]|0;f=a+36|0;g=a+32|0;if((e|0)==(c[f>>2]|0)){a=c[g>>2]|0;c[f>>2]=e<<1;f=en(e<<3)|0;c[g>>2]=f;h=a;On(f|0,h|0,c[d>>2]<<2)|0;fn(h);i=c[d>>2]|0}else{i=e}c[(c[g>>2]|0)+(i<<2)>>2]=b;c[d>>2]=(c[d>>2]|0)+1;return}function ff(a,b){a=a|0;b=b|0;var d=0,e=0,f=0,g=0,h=0,i=0,j=0,k=0,l=0;d=a+56|0;e=c[d>>2]|0;if((e|0)==(b|0)){return 1}f=a+52|0;g=c[f>>2]|0;h=a+48|0;i=a+44|0;if((g|0)==(c[h>>2]|0)){a=c[i>>2]|0;c[h>>2]=g<<1;h=en(g<<4)|0;c[i>>2]=h;j=a;On(h|0,j|0,c[f>>2]<<3)|0;fn(j);k=c[d>>2]|0;l=c[f>>2]|0}else{k=e;l=g}g=c[i>>2]|0;c[g+(l<<3)>>2]=(k|0)>(b|0)?b:k;k=c[d>>2]|0;c[g+(c[f>>2]<<3)+4>>2]=(k|0)<(b|0)?b:k;c[f>>2]=(c[f>>2]|0)+1;return 1}function gf(a,b,d,e,f){a=a|0;b=b|0;d=d|0;e=e|0;f=f|0;var h=0,i=0,j=0.0,k=0.0,l=0.0,m=0.0,n=0,o=0.0,p=0.0,q=0.0,r=0.0,s=0.0,t=0.0;h=a+60|0;c[h>>2]=0;i=b+12|0;j=+g[d+12>>2];k=+g[i>>2];l=+g[d+8>>2];m=+g[b+16>>2];n=e+12|0;o=+g[f+12>>2];p=+g[n>>2];q=+g[f+8>>2];r=+g[e+16>>2];s=+g[f>>2]+(o*p-q*r)-(+g[d>>2]+(j*k-l*m));t=p*q+o*r+ +g[f+4>>2]-(k*l+j*m+ +g[d+4>>2]);m=+g[b+8>>2]+ +g[e+8>>2];if(s*s+t*t>m*m){return}c[a+56>>2]=0;e=i;i=a+48|0;b=c[e+4>>2]|0;c[i>>2]=c[e>>2];c[i+4>>2]=b;g[a+40>>2]=0.0;g[a+44>>2]=0.0;c[h>>2]=1;h=n;n=a;b=c[h+4>>2]|0;c[n>>2]=c[h>>2];c[n+4>>2]=b;c[a+16>>2]=0;return}function hf(a,b,d,e,f){a=a|0;b=b|0;d=d|0;e=e|0;f=f|0;var h=0,i=0,j=0.0,l=0.0,m=0.0,n=0.0,o=0.0,p=0.0,q=0,r=0,s=0.0,t=0,u=0,v=0,w=0,x=0,y=0.0,z=0.0,A=0.0,B=0.0,C=0.0,D=0.0;h=a+60|0;c[h>>2]=0;i=e+12|0;j=+g[f+12>>2];l=+g[i>>2];m=+g[f+8>>2];n=+g[e+16>>2];o=+g[f>>2]+(j*l-m*n)- +g[d>>2];p=l*m+j*n+ +g[f+4>>2]- +g[d+4>>2];n=+g[d+12>>2];j=+g[d+8>>2];m=o*n+p*j;l=n*p+o*(-0.0-j);j=+g[b+8>>2]+ +g[e+8>>2];e=c[b+148>>2]|0;do{if((e|0)>0){d=0;o=-3.4028234663852886e+38;f=0;while(1){p=(m- +g[b+20+(d<<3)>>2])*+g[b+84+(d<<3)>>2]+(l- +g[b+20+(d<<3)+4>>2])*+g[b+84+(d<<3)+4>>2];if(p>j){q=19;break}r=p>o;s=r?p:o;t=r?d:f;r=d+1|0;if((r|0)<(e|0)){d=r;o=s;f=t}else{q=4;break}}if((q|0)==4){u=s<1.1920928955078125e-7;v=t;break}else if((q|0)==19){return}}else{u=1;v=0}}while(0);q=v+1|0;t=b+20+(v<<3)|0;f=c[t>>2]|0;d=c[t+4>>2]|0;s=(c[k>>2]=f,+g[k>>2]);t=d;o=(c[k>>2]=t,+g[k>>2]);r=b+20+(((q|0)<(e|0)?q:0)<<3)|0;q=c[r>>2]|0;e=c[r+4>>2]|0;p=(c[k>>2]=q,+g[k>>2]);r=e;n=(c[k>>2]=r,+g[k>>2]);if(u){c[h>>2]=1;c[a+56>>2]=1;u=b+84+(v<<3)|0;w=a+40|0;x=c[u+4>>2]|0;c[w>>2]=c[u>>2];c[w+4>>2]=x;x=a+48|0;y=+((s+p)*.5);z=+((o+n)*.5);g[x>>2]=y;g[x+4>>2]=z;x=i;w=a;u=c[x+4>>2]|0;c[w>>2]=c[x>>2];c[w+4>>2]=u;c[a+16>>2]=0;return}z=m-s;y=l-o;A=m-p;B=l-n;if(!(z*(p-s)+y*(n-o)>0.0)){if(z*z+y*y>j*j){return}c[h>>2]=1;c[a+56>>2]=1;u=a+40|0;w=u;C=+z;D=+y;g[w>>2]=C;g[w+4>>2]=D;D=+R(z*z+y*y);if(!(D<1.1920928955078125e-7)){C=1.0/D;g[u>>2]=z*C;g[a+44>>2]=y*C}u=a+48|0;c[u>>2]=f|0;c[u+4>>2]=t|d&0;d=i;t=a;u=c[d+4>>2]|0;c[t>>2]=c[d>>2];c[t+4>>2]=u;c[a+16>>2]=0;return}if(A*(s-p)+B*(o-n)>0.0){C=(s+p)*.5;p=(o+n)*.5;u=b+84+(v<<3)|0;if((m-C)*+g[u>>2]+(l-p)*+g[b+84+(v<<3)+4>>2]>j){return}c[h>>2]=1;c[a+56>>2]=1;v=u;u=a+40|0;b=c[v+4>>2]|0;c[u>>2]=c[v>>2];c[u+4>>2]=b;b=a+48|0;l=+C;C=+p;g[b>>2]=l;g[b+4>>2]=C;b=i;u=a;v=c[b+4>>2]|0;c[u>>2]=c[b>>2];c[u+4>>2]=v;c[a+16>>2]=0;return}if(A*A+B*B>j*j){return}c[h>>2]=1;c[a+56>>2]=1;h=a+40|0;v=h;j=+A;C=+B;g[v>>2]=j;g[v+4>>2]=C;C=+R(A*A+B*B);if(!(C<1.1920928955078125e-7)){j=1.0/C;g[h>>2]=A*j;g[a+44>>2]=B*j}h=a+48|0;c[h>>2]=q|0;c[h+4>>2]=r|e&0;e=i;i=a;r=c[e+4>>2]|0;c[i>>2]=c[e>>2];c[i+4>>2]=r;c[a+16>>2]=0;return}function jf(a){a=a|0;var b=0,d=0,e=0,f=0,g=0,h=0,i=0;c[a>>2]=-1;b=a+12|0;c[b>>2]=16;c[a+8>>2]=0;d=en(576)|0;e=a+4|0;c[e>>2]=d;Nn(d|0,0,(c[b>>2]|0)*36|0|0)|0;d=(c[b>>2]|0)-1|0;f=c[e>>2]|0;if((d|0)>0){e=0;while(1){g=e+1|0;c[f+(e*36|0)+20>>2]=g;c[f+(e*36|0)+32>>2]=-1;h=(c[b>>2]|0)-1|0;if((g|0)<(h|0)){e=g}else{i=h;break}}}else{i=d}c[f+(i*36|0)+20>>2]=-1;c[f+(((c[b>>2]|0)-1|0)*36|0)+32>>2]=-1;c[a+16>>2]=0;c[a+20>>2]=0;c[a+24>>2]=0;return}function kf(a){a=a|0;fn(c[a+4>>2]|0);return}function lf(a){a=a|0;var b=0,d=0,e=0,f=0,g=0,h=0,i=0,j=0,k=0,l=0,m=0,n=0,o=0,p=0;b=a+16|0;d=c[b>>2]|0;e=a+4|0;f=c[e>>2]|0;if((d|0)==-1){g=a+12|0;h=c[g>>2]|0;c[g>>2]=h<<1;i=en(h*72|0)|0;c[e>>2]=i;h=f;j=a+8|0;On(i|0,h|0,(c[j>>2]|0)*36|0)|0;fn(h);h=c[j>>2]|0;i=(c[g>>2]|0)-1|0;k=c[e>>2]|0;if((h|0)<(i|0)){e=h;while(1){h=e+1|0;c[k+(e*36|0)+20>>2]=h;c[k+(e*36|0)+32>>2]=-1;l=(c[g>>2]|0)-1|0;if((h|0)<(l|0)){e=h}else{m=l;break}}}else{m=i}c[k+(m*36|0)+20>>2]=-1;c[k+(((c[g>>2]|0)-1|0)*36|0)+32>>2]=-1;g=c[j>>2]|0;c[b>>2]=g;n=g;o=k;p=j}else{n=d;o=f;p=a+8|0}a=o+(n*36|0)+20|0;c[b>>2]=c[a>>2];c[a>>2]=-1;c[o+(n*36|0)+24>>2]=-1;c[o+(n*36|0)+28>>2]=-1;c[o+(n*36|0)+32>>2]=0;c[o+(n*36|0)+16>>2]=0;c[p>>2]=(c[p>>2]|0)+1;return n|0}function mf(a,b,d){a=a|0;b=b|0;d=d|0;var e=0,f=0,h=0,i=0.0,j=0.0;e=lf(a)|0;f=a+4|0;h=(c[f>>2]|0)+(e*36|0)|0;i=+(+g[b>>2]+-.10000000149011612);j=+(+g[b+4>>2]+-.10000000149011612);g[h>>2]=i;g[h+4>>2]=j;h=(c[f>>2]|0)+(e*36|0)+8|0;j=+(+g[b+8>>2]+.10000000149011612);i=+(+g[b+12>>2]+.10000000149011612);g[h>>2]=j;g[h+4>>2]=i;c[(c[f>>2]|0)+(e*36|0)+16>>2]=d;c[(c[f>>2]|0)+(e*36|0)+32>>2]=0;nf(a,e);return e|0}function nf(a,b){a=a|0;b=b|0;var d=0,e=0,f=0,h=0,i=0.0,j=0.0,k=0.0,l=0.0,m=0,n=0,o=0,p=0,q=0,r=0.0,s=0.0,t=0.0,u=0.0,v=0.0,w=0.0,x=0.0,y=0.0,z=0.0,A=0.0,B=0.0,C=0.0,D=0.0,E=0,F=0,G=0;d=a+24|0;c[d>>2]=(c[d>>2]|0)+1;d=a|0;e=c[d>>2]|0;if((e|0)==-1){c[d>>2]=b;c[(c[a+4>>2]|0)+(b*36|0)+20>>2]=-1;return}f=a+4|0;h=c[f>>2]|0;i=+g[h+(b*36|0)>>2];j=+g[h+(b*36|0)+4>>2];k=+g[h+(b*36|0)+8>>2];l=+g[h+(b*36|0)+12>>2];m=c[h+(e*36|0)+24>>2]|0;a:do{if((m|0)==-1){n=e}else{o=e;p=m;while(1){q=c[h+(o*36|0)+28>>2]|0;r=+g[h+(o*36|0)+8>>2];s=+g[h+(o*36|0)>>2];t=+g[h+(o*36|0)+12>>2];u=+g[h+(o*36|0)+4>>2];v=((r>k?r:k)-(s<i?s:i)+((t>l?t:l)-(u<j?u:j)))*2.0;w=v*2.0;x=(v-(r-s+(t-u))*2.0)*2.0;u=+g[h+(p*36|0)>>2];t=i<u?i:u;s=+g[h+(p*36|0)+4>>2];r=j<s?j:s;v=+g[h+(p*36|0)+8>>2];y=k>v?k:v;z=+g[h+(p*36|0)+12>>2];A=l>z?l:z;if((c[h+(p*36|0)+24>>2]|0)==-1){B=(y-t+(A-r))*2.0}else{B=(y-t+(A-r))*2.0-(v-u+(z-s))*2.0}s=x+B;z=+g[h+(q*36|0)>>2];u=i<z?i:z;v=+g[h+(q*36|0)+4>>2];r=j<v?j:v;A=+g[h+(q*36|0)+8>>2];t=k>A?k:A;y=+g[h+(q*36|0)+12>>2];C=l>y?l:y;if((c[h+(q*36|0)+24>>2]|0)==-1){D=(t-u+(C-r))*2.0}else{D=(t-u+(C-r))*2.0-(A-z+(y-v))*2.0}v=x+D;if(w<s&w<v){n=o;break a}E=s<v?p:q;q=c[h+(E*36|0)+24>>2]|0;if((q|0)==-1){n=E;break}else{o=E;p=q}}}}while(0);m=c[h+(n*36|0)+20>>2]|0;h=lf(a)|0;e=c[f>>2]|0;c[e+(h*36|0)+20>>2]=m;c[e+(h*36|0)+16>>2]=0;e=c[f>>2]|0;D=+g[e+(n*36|0)>>2];B=+g[e+(n*36|0)+4>>2];p=e+(h*36|0)|0;v=+(i<D?i:D);D=+(j<B?j:B);g[p>>2]=v;g[p+4>>2]=D;D=+g[e+(n*36|0)+8>>2];v=+g[e+(n*36|0)+12>>2];p=e+(h*36|0)+8|0;B=+(k>D?k:D);D=+(l>v?l:v);g[p>>2]=B;g[p+4>>2]=D;p=c[f>>2]|0;c[p+(h*36|0)+32>>2]=(c[p+(n*36|0)+32>>2]|0)+1;if((m|0)==-1){c[p+(h*36|0)+24>>2]=n;c[p+(h*36|0)+28>>2]=b;c[p+(n*36|0)+20>>2]=h;e=p+(b*36|0)+20|0;c[e>>2]=h;c[d>>2]=h;F=c[e>>2]|0}else{e=p+(m*36|0)+24|0;if((c[e>>2]|0)==(n|0)){c[e>>2]=h}else{c[p+(m*36|0)+28>>2]=h}c[p+(h*36|0)+24>>2]=n;c[p+(h*36|0)+28>>2]=b;c[p+(n*36|0)+20>>2]=h;c[p+(b*36|0)+20>>2]=h;F=h}if((F|0)==-1){return}else{G=F}do{F=rf(a,G)|0;h=c[f>>2]|0;b=c[h+(F*36|0)+24>>2]|0;p=c[h+(F*36|0)+28>>2]|0;n=c[h+(b*36|0)+32>>2]|0;m=c[h+(p*36|0)+32>>2]|0;c[h+(F*36|0)+32>>2]=((n|0)>(m|0)?n:m)+1;D=+g[h+(b*36|0)>>2];B=+g[h+(p*36|0)>>2];v=+g[h+(b*36|0)+4>>2];l=+g[h+(p*36|0)+4>>2];m=h+(F*36|0)|0;k=+(D<B?D:B);B=+(v<l?v:l);g[m>>2]=k;g[m+4>>2]=B;B=+g[h+(b*36|0)+8>>2];k=+g[h+(p*36|0)+8>>2];l=+g[h+(b*36|0)+12>>2];v=+g[h+(p*36|0)+12>>2];p=h+(F*36|0)+8|0;D=+(B>k?B:k);k=+(l>v?l:v);g[p>>2]=D;g[p+4>>2]=k;G=c[(c[f>>2]|0)+(F*36|0)+20>>2]|0;}while(!((G|0)==-1));return}function of(a,b){a=a|0;b=b|0;var d=0,e=0;pf(a,b);d=a+16|0;e=c[a+4>>2]|0;c[e+(b*36|0)+20>>2]=c[d>>2];c[e+(b*36|0)+32>>2]=-1;c[d>>2]=b;b=a+8|0;c[b>>2]=(c[b>>2]|0)-1;return}function pf(a,b){a=a|0;b=b|0;var d=0,e=0,f=0,h=0,i=0,j=0,k=0,l=0,m=0.0,n=0.0,o=0.0,p=0.0,q=0.0;d=a|0;if((c[d>>2]|0)==(b|0)){c[d>>2]=-1;return}e=a+4|0;f=c[e>>2]|0;h=c[f+(b*36|0)+20>>2]|0;i=f+(h*36|0)+20|0;j=c[i>>2]|0;k=c[f+(h*36|0)+24>>2]|0;if((k|0)==(b|0)){l=c[f+(h*36|0)+28>>2]|0}else{l=k}if((j|0)==-1){c[d>>2]=l;c[f+(l*36|0)+20>>2]=-1;d=a+16|0;c[i>>2]=c[d>>2];c[f+(h*36|0)+32>>2]=-1;c[d>>2]=h;d=a+8|0;c[d>>2]=(c[d>>2]|0)-1;return}d=f+(j*36|0)+24|0;if((c[d>>2]|0)==(h|0)){c[d>>2]=l}else{c[f+(j*36|0)+28>>2]=l}c[f+(l*36|0)+20>>2]=j;l=a+16|0;c[i>>2]=c[l>>2];c[f+(h*36|0)+32>>2]=-1;c[l>>2]=h;h=a+8|0;c[h>>2]=(c[h>>2]|0)-1;h=j;do{j=rf(a,h)|0;l=c[e>>2]|0;f=c[l+(j*36|0)+24>>2]|0;i=c[l+(j*36|0)+28>>2]|0;m=+g[l+(f*36|0)>>2];n=+g[l+(i*36|0)>>2];o=+g[l+(f*36|0)+4>>2];p=+g[l+(i*36|0)+4>>2];d=l+(j*36|0)|0;q=+(m<n?m:n);n=+(o<p?o:p);g[d>>2]=q;g[d+4>>2]=n;n=+g[l+(f*36|0)+8>>2];q=+g[l+(i*36|0)+8>>2];p=+g[l+(f*36|0)+12>>2];o=+g[l+(i*36|0)+12>>2];d=l+(j*36|0)+8|0;m=+(n>q?n:q);q=+(p>o?p:o);g[d>>2]=m;g[d+4>>2]=q;d=c[e>>2]|0;l=c[d+(f*36|0)+32>>2]|0;f=c[d+(i*36|0)+32>>2]|0;c[d+(j*36|0)+32>>2]=((l|0)>(f|0)?l:f)+1;h=c[d+(j*36|0)+20>>2]|0;}while(!((h|0)==-1));return}function qf(a,b,d,e){a=a|0;b=b|0;d=d|0;e=e|0;var f=0,h=0,i=0,j=0.0,k=0.0,l=0.0,m=0.0,n=0.0,o=0.0,p=0.0,q=0.0,r=0.0,s=0.0;f=a+4|0;h=c[f>>2]|0;do{if(!(+g[h+(b*36|0)>>2]>+g[d>>2])){if(+g[h+(b*36|0)+4>>2]>+g[d+4>>2]){break}if(+g[d+8>>2]>+g[h+(b*36|0)+8>>2]){break}if(+g[d+12>>2]>+g[h+(b*36|0)+12>>2]){break}else{i=0}return i|0}}while(0);pf(a,b);h=d;j=+g[h>>2];k=+g[h+4>>2];h=d+8|0;l=+g[h>>2];m=j+-.10000000149011612;j=k+-.10000000149011612;k=l+.10000000149011612;l=+g[h+4>>2]+.10000000149011612;n=+g[e>>2]*2.0;o=+g[e+4>>2]*2.0;if(n<0.0){p=k;q=m+n}else{p=n+k;q=m}if(o<0.0){r=l;s=j+o}else{r=o+l;s=j}e=c[f>>2]|0;f=e+(b*36|0)|0;j=+q;q=+s;g[f>>2]=j;g[f+4>>2]=q;f=e+(b*36|0)+8|0;q=+p;p=+r;g[f>>2]=q;g[f+4>>2]=p;nf(a,b);i=1;return i|0}function rf(a,b){a=a|0;b=b|0;var d=0,e=0,f=0,h=0,i=0,j=0,k=0,l=0,m=0,n=0,o=0,p=0,q=0,r=0,s=0,t=0,u=0,v=0,w=0,x=0,y=0.0,z=0.0,A=0.0,B=0,C=0.0,D=0.0,E=0.0,F=0,G=0,H=0,I=0;d=c[a+4>>2]|0;e=d+(b*36|0)|0;f=d+(b*36|0)+24|0;h=c[f>>2]|0;if((h|0)==-1){i=b;return i|0}j=d+(b*36|0)+32|0;if((c[j>>2]|0)<2){i=b;return i|0}k=d+(b*36|0)+28|0;l=c[k>>2]|0;m=d+(h*36|0)|0;n=d+(l*36|0)|0;o=d+(l*36|0)+32|0;p=d+(h*36|0)+32|0;q=(c[o>>2]|0)-(c[p>>2]|0)|0;if((q|0)>1){r=d+(l*36|0)+24|0;s=c[r>>2]|0;t=d+(l*36|0)+28|0;u=c[t>>2]|0;v=d+(s*36|0)|0;w=d+(u*36|0)|0;c[r>>2]=b;r=d+(b*36|0)+20|0;x=d+(l*36|0)+20|0;c[x>>2]=c[r>>2];c[r>>2]=l;r=c[x>>2]|0;do{if((r|0)==-1){c[a>>2]=l}else{x=d+(r*36|0)+24|0;if((c[x>>2]|0)==(b|0)){c[x>>2]=l;break}else{c[d+(r*36|0)+28>>2]=l;break}}}while(0);r=d+(s*36|0)+32|0;x=d+(u*36|0)+32|0;if((c[r>>2]|0)>(c[x>>2]|0)){c[t>>2]=s;c[k>>2]=u;c[d+(u*36|0)+20>>2]=b;y=+g[m>>2];z=+g[w>>2];A=y<z?y:z;z=+g[d+(h*36|0)+4>>2];y=+g[d+(u*36|0)+4>>2];B=e;C=+A;D=+(z<y?z:y);g[B>>2]=C;g[B+4>>2]=D;D=+g[d+(h*36|0)+8>>2];C=+g[d+(u*36|0)+8>>2];y=+g[d+(h*36|0)+12>>2];z=+g[d+(u*36|0)+12>>2];B=d+(b*36|0)+8|0;E=+(D>C?D:C);C=+(y>z?y:z);g[B>>2]=E;g[B+4>>2]=C;C=+g[v>>2];E=+g[d+(b*36|0)+4>>2];z=+g[d+(s*36|0)+4>>2];B=n;y=+(A<C?A:C);C=+(E<z?E:z);g[B>>2]=y;g[B+4>>2]=C;C=+g[d+(b*36|0)+8>>2];y=+g[d+(s*36|0)+8>>2];z=+g[d+(b*36|0)+12>>2];E=+g[d+(s*36|0)+12>>2];B=d+(l*36|0)+8|0;A=+(C>y?C:y);y=+(z>E?z:E);g[B>>2]=A;g[B+4>>2]=y;B=c[p>>2]|0;F=c[x>>2]|0;G=((B|0)>(F|0)?B:F)+1|0;c[j>>2]=G;F=c[r>>2]|0;H=(G|0)>(F|0)?G:F}else{c[t>>2]=u;c[k>>2]=s;c[d+(s*36|0)+20>>2]=b;y=+g[m>>2];A=+g[v>>2];E=y<A?y:A;A=+g[d+(h*36|0)+4>>2];y=+g[d+(s*36|0)+4>>2];v=e;z=+E;C=+(A<y?A:y);g[v>>2]=z;g[v+4>>2]=C;C=+g[d+(h*36|0)+8>>2];z=+g[d+(s*36|0)+8>>2];y=+g[d+(h*36|0)+12>>2];A=+g[d+(s*36|0)+12>>2];s=d+(b*36|0)+8|0;D=+(C>z?C:z);z=+(y>A?y:A);g[s>>2]=D;g[s+4>>2]=z;z=+g[w>>2];D=+g[d+(b*36|0)+4>>2];A=+g[d+(u*36|0)+4>>2];w=n;y=+(E<z?E:z);z=+(D<A?D:A);g[w>>2]=y;g[w+4>>2]=z;z=+g[d+(b*36|0)+8>>2];y=+g[d+(u*36|0)+8>>2];A=+g[d+(b*36|0)+12>>2];D=+g[d+(u*36|0)+12>>2];u=d+(l*36|0)+8|0;E=+(z>y?z:y);y=+(A>D?A:D);g[u>>2]=E;g[u+4>>2]=y;u=c[p>>2]|0;w=c[r>>2]|0;r=((u|0)>(w|0)?u:w)+1|0;c[j>>2]=r;w=c[x>>2]|0;H=(r|0)>(w|0)?r:w}c[o>>2]=H+1;i=l;return i|0}if(!((q|0)<-1)){i=b;return i|0}q=d+(h*36|0)+24|0;H=c[q>>2]|0;w=d+(h*36|0)+28|0;r=c[w>>2]|0;x=d+(H*36|0)|0;u=d+(r*36|0)|0;c[q>>2]=b;q=d+(b*36|0)+20|0;s=d+(h*36|0)+20|0;c[s>>2]=c[q>>2];c[q>>2]=h;q=c[s>>2]|0;do{if((q|0)==-1){c[a>>2]=h}else{s=d+(q*36|0)+24|0;if((c[s>>2]|0)==(b|0)){c[s>>2]=h;break}else{c[d+(q*36|0)+28>>2]=h;break}}}while(0);q=d+(H*36|0)+32|0;a=d+(r*36|0)+32|0;if((c[q>>2]|0)>(c[a>>2]|0)){c[w>>2]=H;c[f>>2]=r;c[d+(r*36|0)+20>>2]=b;y=+g[n>>2];E=+g[u>>2];D=y<E?y:E;E=+g[d+(l*36|0)+4>>2];y=+g[d+(r*36|0)+4>>2];s=e;A=+D;z=+(E<y?E:y);g[s>>2]=A;g[s+4>>2]=z;z=+g[d+(l*36|0)+8>>2];A=+g[d+(r*36|0)+8>>2];y=+g[d+(l*36|0)+12>>2];E=+g[d+(r*36|0)+12>>2];s=d+(b*36|0)+8|0;C=+(z>A?z:A);A=+(y>E?y:E);g[s>>2]=C;g[s+4>>2]=A;A=+g[x>>2];C=+g[d+(b*36|0)+4>>2];E=+g[d+(H*36|0)+4>>2];s=m;y=+(D<A?D:A);A=+(C<E?C:E);g[s>>2]=y;g[s+4>>2]=A;A=+g[d+(b*36|0)+8>>2];y=+g[d+(H*36|0)+8>>2];E=+g[d+(b*36|0)+12>>2];C=+g[d+(H*36|0)+12>>2];s=d+(h*36|0)+8|0;D=+(A>y?A:y);y=+(E>C?E:C);g[s>>2]=D;g[s+4>>2]=y;s=c[o>>2]|0;v=c[a>>2]|0;k=((s|0)>(v|0)?s:v)+1|0;c[j>>2]=k;v=c[q>>2]|0;I=(k|0)>(v|0)?k:v}else{c[w>>2]=r;c[f>>2]=H;c[d+(H*36|0)+20>>2]=b;y=+g[n>>2];D=+g[x>>2];C=y<D?y:D;D=+g[d+(l*36|0)+4>>2];y=+g[d+(H*36|0)+4>>2];x=e;E=+C;A=+(D<y?D:y);g[x>>2]=E;g[x+4>>2]=A;A=+g[d+(l*36|0)+8>>2];E=+g[d+(H*36|0)+8>>2];y=+g[d+(l*36|0)+12>>2];D=+g[d+(H*36|0)+12>>2];H=d+(b*36|0)+8|0;z=+(A>E?A:E);E=+(y>D?y:D);g[H>>2]=z;g[H+4>>2]=E;E=+g[u>>2];z=+g[d+(b*36|0)+4>>2];D=+g[d+(r*36|0)+4>>2];u=m;y=+(C<E?C:E);E=+(z<D?z:D);g[u>>2]=y;g[u+4>>2]=E;E=+g[d+(b*36|0)+8>>2];y=+g[d+(r*36|0)+8>>2];D=+g[d+(b*36|0)+12>>2];z=+g[d+(r*36|0)+12>>2];r=d+(h*36|0)+8|0;C=+(E>y?E:y);y=+(D>z?D:z);g[r>>2]=C;g[r+4>>2]=y;r=c[o>>2]|0;o=c[q>>2]|0;q=((r|0)>(o|0)?r:o)+1|0;c[j>>2]=q;j=c[a>>2]|0;I=(q|0)>(j|0)?q:j}c[p>>2]=I+1;i=h;return i|0}function sf(b,d,e,f,h){b=b|0;d=d|0;e=e|0;f=f|0;h=h|0;var i=0,j=0,l=0.0,m=0.0,n=0.0,o=0.0,p=0.0,q=0.0,r=0,s=0,t=0,u=0,v=0.0,w=0.0,x=0.0,y=0.0,z=0.0,A=0.0,B=0.0,C=0.0,D=0,E=0,F=0.0,G=0.0,H=0.0,I=0.0,J=0.0,K=0.0;i=b+60|0;c[i>>2]=0;j=f+12|0;l=+g[h+12>>2];m=+g[j>>2];n=+g[h+8>>2];o=+g[f+16>>2];p=+g[h>>2]+(l*m-n*o)- +g[e>>2];q=m*n+l*o+ +g[h+4>>2]- +g[e+4>>2];o=+g[e+12>>2];l=+g[e+8>>2];n=p*o+q*l;m=o*q+p*(-0.0-l);e=d+12|0;h=c[e>>2]|0;r=c[e+4>>2]|0;l=(c[k>>2]=h,+g[k>>2]);e=r;p=(c[k>>2]=e,+g[k>>2]);s=d+20|0;t=c[s>>2]|0;u=c[s+4>>2]|0;q=(c[k>>2]=t,+g[k>>2]);s=u;o=(c[k>>2]=s,+g[k>>2]);v=q-l;w=o-p;x=v*(q-n)+w*(o-m);y=n-l;z=m-p;A=y*v+z*w;B=+g[d+8>>2]+ +g[f+8>>2];if(!(A>0.0)){if(y*y+z*z>B*B){return}do{if((a[d+44|0]|0)!=0){f=d+28|0;C=+g[f>>2];if(!((l-n)*(l-C)+(p-m)*(p- +g[f+4>>2])>0.0)){break}return}}while(0);c[i>>2]=1;c[b+56>>2]=0;g[b+40>>2]=0.0;g[b+44>>2]=0.0;f=b+48|0;c[f>>2]=h|0;c[f+4>>2]=e|r&0;f=b+16|0;c[f>>2]=0;D=f;a[f]=0;a[D+1|0]=0;a[D+2|0]=0;a[D+3|0]=0;D=j;f=b;E=c[D+4>>2]|0;c[f>>2]=c[D>>2];c[f+4>>2]=E;return}if(!(x>0.0)){C=n-q;F=m-o;if(C*C+F*F>B*B){return}do{if((a[d+45|0]|0)!=0){E=d+36|0;G=+g[E>>2];if(!(C*(G-q)+F*(+g[E+4>>2]-o)>0.0)){break}return}}while(0);c[i>>2]=1;c[b+56>>2]=0;g[b+40>>2]=0.0;g[b+44>>2]=0.0;d=b+48|0;c[d>>2]=t|0;c[d+4>>2]=s|u&0;u=b+16|0;c[u>>2]=0;s=u;a[u]=1;a[s+1|0]=0;a[s+2|0]=0;a[s+3|0]=0;s=j;u=b;d=c[s+4>>2]|0;c[u>>2]=c[s>>2];c[u+4>>2]=d;return}F=1.0/(v*v+w*w);C=n-(l*x+q*A)*F;q=m-(p*x+o*A)*F;if(C*C+q*q>B*B){return}B=-0.0-w;if(v*z+y*B<0.0){H=w;I=-0.0-v}else{H=B;I=v}v=+R(I*I+H*H);if(v<1.1920928955078125e-7){J=H;K=I}else{B=1.0/v;J=H*B;K=I*B}c[i>>2]=1;c[b+56>>2]=1;i=b+40|0;B=+J;J=+K;g[i>>2]=B;g[i+4>>2]=J;i=b+48|0;c[i>>2]=h|0;c[i+4>>2]=e|r&0;r=b+16|0;c[r>>2]=0;e=r;a[r]=0;a[e+1|0]=0;a[e+2|0]=1;a[e+3|0]=0;e=j;j=b;b=c[e+4>>2]|0;c[j>>2]=c[e>>2];c[j+4>>2]=b;return}



function Xm(a,b,c){a=a|0;b=b|0;c=c|0;var d=0.0,e=0.0,f=0.0,h=0.0,i=0.0,j=0.0,k=0.0,l=0.0,m=0.0,n=0.0,o=0.0,p=0.0,q=0.0,r=0.0,s=0.0,t=0.0;d=+g[b+16>>2];e=+g[b+32>>2];f=+g[b+20>>2];h=+g[b+28>>2];i=d*e-f*h;j=+g[b+24>>2];k=+g[b+12>>2];l=f*j-e*k;m=h*k-d*j;n=+g[b>>2];o=+g[b+4>>2];p=+g[b+8>>2];q=i*n+o*l+m*p;if(q!=0.0){r=1.0/q}else{r=q}q=+g[c>>2];s=+g[c+4>>2];t=+g[c+8>>2];g[a>>2]=r*(i*q+s*l+m*t);g[a+4>>2]=r*((s*e-t*h)*n+o*(t*j-e*q)+(h*q-s*j)*p);g[a+8>>2]=r*((d*t-f*s)*n+o*(f*q-t*k)+(s*k-d*q)*p);return}function Ym(a,b,c){a=a|0;b=b|0;c=c|0;var d=0.0,e=0.0,f=0.0,h=0.0,i=0.0,j=0.0,k=0.0;d=+g[b>>2];e=+g[b+12>>2];f=+g[b+4>>2];h=+g[b+16>>2];i=d*h-e*f;if(i!=0.0){j=1.0/i}else{j=i}i=+g[c>>2];k=+g[c+4>>2];g[a>>2]=j*(h*i-e*k);g[a+4>>2]=j*(d*k-f*i);return}function Zm(a,b){a=a|0;b=b|0;var c=0.0,d=0.0,e=0.0,f=0.0,h=0.0,i=0.0;c=+g[a>>2];d=+g[a+12>>2];e=+g[a+4>>2];f=+g[a+16>>2];h=c*f-d*e;if(h!=0.0){i=1.0/h}else{i=h}g[b>>2]=f*i;f=-0.0-i;g[b+12>>2]=d*f;g[b+8>>2]=0.0;g[b+4>>2]=e*f;g[b+16>>2]=c*i;Nn(b+20|0,0,16)|0;return}function _m(a,b){a=a|0;b=b|0;var c=0.0,d=0.0,e=0.0,f=0.0,h=0.0,i=0.0,j=0.0,k=0.0,l=0.0,m=0.0,n=0.0,o=0.0;c=+g[a+16>>2];d=+g[a+32>>2];e=c*d;f=+g[a+20>>2];h=+g[a+28>>2];i=+g[a+24>>2];j=+g[a+12>>2];k=d*j;l=h*j-c*i;m=+g[a>>2];n=(e-f*h)*m+ +g[a+4>>2]*(f*i-k)+l*+g[a+8>>2];if(n!=0.0){o=1.0/n}else{o=n}g[b>>2]=o*(e-h*h);e=o*(i*h-k);g[b+4>>2]=e;k=o*l;g[b+8>>2]=k;g[b+12>>2]=e;g[b+16>>2]=o*(m*d-i*i);d=o*(j*i-m*h);g[b+20>>2]=d;g[b+24>>2]=k;g[b+28>>2]=d;g[b+32>>2]=o*(m*c-j*j);return}function $m(a){a=a|0;return}function an(a){a=a|0;return}function bn(a){a=a|0;return+0.0}function cn(a,b){a=a|0;b=b|0;return Fn(a)|0}function dn(a,b){a=a|0;b=b|0;Gn(a);return}function en(a){a=a|0;c[2490]=(c[2490]|0)+1;return tb[c[2110]&63](a,c[2486]|0)|0}function fn(a){a=a|0;c[2490]=(c[2490]|0)-1;kb[c[2112]&255](a,c[2486]|0);return}function gn(a,b){a=a|0;b=b|0;return}function hn(a){a=a|0;return a+43&-32|0}function jn(a,b){a=a|0;b=b|0;var d=0,e=0,f=0;d=en(b+44|0)|0;c[d>>2]=d;b=d+43&-32;c[b-4>>2]=d;e=d;c[d+4>>2]=a;f=a|0;c[d>>2]=c[f>>2];c[(c[f>>2]|0)+4>>2]=e;c[f>>2]=e;return b|0}function kn(a,b){a=a|0;b=b|0;var d=0,e=0;a=c[b-4>>2]|0;b=a|0;d=a+4|0;e=a|0;c[(c[e>>2]|0)+4>>2]=c[d>>2];c[c[d>>2]>>2]=c[e>>2];c[d>>2]=b;c[e>>2]=b;fn(a);return}function ln(a){a=a|0;var b=0,d=0,e=0,f=0;b=a|0;d=a+4|0;a=c[d>>2]|0;if((a|0)==(b|0)){return}else{e=a}do{a=e+4|0;f=e|0;c[(c[f>>2]|0)+4>>2]=c[a>>2];c[c[a>>2]>>2]=c[f>>2];c[a>>2]=e;c[f>>2]=e;fn(e);e=c[d>>2]|0;}while((e|0)!=(b|0));return}function mn(a){a=a|0;c[a+102400>>2]=0;c[a+102404>>2]=0;c[a+102408>>2]=0;c[a+102796>>2]=0;return}function nn(a){a=a|0;return}function on(b,d){b=b|0;d=d|0;var e=0,f=0,g=0,h=0,i=0;e=b+102796|0;f=c[e>>2]|0;g=b+102412+(f*12|0)|0;c[b+102412+(f*12|0)+4>>2]=d;h=b+102400|0;i=c[h>>2]|0;if((i+d|0)>102400){c[g>>2]=en(d)|0;a[b+102412+(f*12|0)+8|0]=1}else{c[g>>2]=b+i;a[b+102412+(f*12|0)+8|0]=0;c[h>>2]=(c[h>>2]|0)+d}h=b+102404|0;f=(c[h>>2]|0)+d|0;c[h>>2]=f;h=b+102408|0;b=c[h>>2]|0;c[h>>2]=(b|0)>(f|0)?b:f;c[e>>2]=(c[e>>2]|0)+1;return c[g>>2]|0}function pn(b,d,e){b=b|0;d=d|0;e=e|0;var f=0,g=0,h=0,i=0,j=0,k=0,l=0,m=0,n=0;d=(c[b+102796>>2]|0)-1|0;f=b+102412+(d*12|0)|0;g=b+102412+(d*12|0)+4|0;h=e-(c[g>>2]|0)|0;if((h|0)<=0){i=f|0;j=c[i>>2]|0;return j|0}k=b+102412+(d*12|0)+8|0;do{if((a[k]|0)==0){d=b+102400|0;l=(c[d>>2]|0)+h|0;if((l|0)>102400){m=en(e)|0;n=f|0;On(m|0,c[n>>2]|0,c[g>>2]|0)|0;c[d>>2]=(c[d>>2]|0)-(c[g>>2]|0);c[n>>2]=m;a[k]=1;break}else{c[d>>2]=l;l=b+102404|0;d=(c[l>>2]|0)+h|0;c[l>>2]=d;l=b+102408|0;m=c[l>>2]|0;c[l>>2]=(m|0)>(d|0)?m:d;break}}else{d=en(e)|0;m=f|0;On(d|0,c[m>>2]|0,c[g>>2]|0)|0;fn(c[m>>2]|0);c[m>>2]=d}}while(0);c[g>>2]=e;i=f|0;j=c[i>>2]|0;return j|0}function qn(b,d){b=b|0;d=d|0;var e=0,f=0,g=0,h=0,i=0,j=0,k=0;e=b+102796|0;f=c[e>>2]|0;g=f-1|0;if((a[b+102412+(g*12|0)+8|0]|0)==0){h=b+102412+(g*12|0)+4|0;i=b+102400|0;c[i>>2]=(c[i>>2]|0)-(c[h>>2]|0);j=f;k=h}else{fn(d);j=c[e>>2]|0;k=b+102412+(g*12|0)+4|0}g=b+102404|0;c[g>>2]=(c[g>>2]|0)-(c[k>>2]|0);c[e>>2]=j-1;return}function rn(a){a=a|0;return}function sn(a){a=a|0;rn(a|0);return}function tn(a){a=a|0;return}function un(a){a=a|0;return}function vn(a){a=a|0;rn(a|0);Jn(a);return}function wn(a){a=a|0;rn(a|0);Jn(a);return}function xn(a,b,d){a=a|0;b=b|0;d=d|0;var e=0,f=0,g=0,h=0;e=i;i=i+56|0;f=e|0;if((a|0)==(b|0)){g=1;i=e;return g|0}if((b|0)==0){g=0;i=e;return g|0}h=An(b,7888,7872,-1)|0;b=h;if((h|0)==0){g=0;i=e;return g|0}Nn(f|0,0,56)|0;c[f>>2]=b;c[f+8>>2]=a;c[f+12>>2]=-1;c[f+48>>2]=1;wb[c[(c[h>>2]|0)+28>>2]&63](b,f,c[d>>2]|0,1);if((c[f+24>>2]|0)!=1){g=0;i=e;return g|0}c[d>>2]=c[f+16>>2];g=1;i=e;return g|0}function yn(b,d,e,f){b=b|0;d=d|0;e=e|0;f=f|0;var g=0;if((c[d+8>>2]|0)!=(b|0)){return}b=d+16|0;g=c[b>>2]|0;if((g|0)==0){c[b>>2]=e;c[d+24>>2]=f;c[d+36>>2]=1;return}if((g|0)!=(e|0)){e=d+36|0;c[e>>2]=(c[e>>2]|0)+1;c[d+24>>2]=2;a[d+54|0]=1;return}e=d+24|0;if((c[e>>2]|0)!=2){return}c[e>>2]=f;return}function zn(b,d,e,f){b=b|0;d=d|0;e=e|0;f=f|0;var g=0;if((b|0)!=(c[d+8>>2]|0)){g=c[b+8>>2]|0;wb[c[(c[g>>2]|0)+28>>2]&63](g,d,e,f);return}g=d+16|0;b=c[g>>2]|0;if((b|0)==0){c[g>>2]=e;c[d+24>>2]=f;c[d+36>>2]=1;return}if((b|0)!=(e|0)){e=d+36|0;c[e>>2]=(c[e>>2]|0)+1;c[d+24>>2]=2;a[d+54|0]=1;return}e=d+24|0;if((c[e>>2]|0)!=2){return}c[e>>2]=f;return}function An(a,b,d,e){a=a|0;b=b|0;d=d|0;e=e|0;var f=0,g=0,h=0,j=0,k=0,l=0,m=0,n=0,o=0;f=i;i=i+56|0;g=f|0;h=c[a>>2]|0;j=a+(c[h-8>>2]|0)|0;k=c[h-4>>2]|0;h=k;c[g>>2]=d;c[g+4>>2]=a;c[g+8>>2]=b;c[g+12>>2]=e;e=g+16|0;b=g+20|0;a=g+24|0;l=g+28|0;m=g+32|0;n=g+40|0;Nn(e|0,0,39)|0;if((k|0)==(d|0)){c[g+48>>2]=1;sb[c[(c[k>>2]|0)+20>>2]&31](h,g,j,j,1,0);i=f;return((c[a>>2]|0)==1?j:0)|0}hb[c[(c[k>>2]|0)+24>>2]&7](h,g,j,1,0);j=c[g+36>>2]|0;if((j|0)==1){do{if((c[a>>2]|0)!=1){if((c[n>>2]|0)!=0){o=0;i=f;return o|0}if((c[l>>2]|0)!=1){o=0;i=f;return o|0}if((c[m>>2]|0)==1){break}else{o=0}i=f;return o|0}}while(0);o=c[e>>2]|0;i=f;return o|0}else if((j|0)==0){if((c[n>>2]|0)!=1){o=0;i=f;return o|0}if((c[l>>2]|0)!=1){o=0;i=f;return o|0}o=(c[m>>2]|0)==1?c[b>>2]|0:0;i=f;return o|0}else{o=0;i=f;return o|0}return 0}function Bn(b,d,e,f,g){b=b|0;d=d|0;e=e|0;f=f|0;g=g|0;var h=0,i=0,j=0,k=0,l=0,m=0,n=0;h=b|0;if((h|0)==(c[d+8>>2]|0)){if((c[d+4>>2]|0)!=(e|0)){return}i=d+28|0;if((c[i>>2]|0)==1){return}c[i>>2]=f;return}if((h|0)!=(c[d>>2]|0)){h=c[b+8>>2]|0;hb[c[(c[h>>2]|0)+24>>2]&7](h,d,e,f,g);return}do{if((c[d+16>>2]|0)!=(e|0)){h=d+20|0;if((c[h>>2]|0)==(e|0)){break}c[d+32>>2]=f;i=d+44|0;if((c[i>>2]|0)==4){return}j=d+52|0;a[j]=0;k=d+53|0;a[k]=0;l=c[b+8>>2]|0;sb[c[(c[l>>2]|0)+20>>2]&31](l,d,e,e,1,g);if((a[k]|0)==0){m=0;n=13}else{if((a[j]|0)==0){m=1;n=13}}a:do{if((n|0)==13){c[h>>2]=e;j=d+40|0;c[j>>2]=(c[j>>2]|0)+1;do{if((c[d+36>>2]|0)==1){if((c[d+24>>2]|0)!=2){n=16;break}a[d+54|0]=1;if(m){break a}}else{n=16}}while(0);if((n|0)==16){if(m){break}}c[i>>2]=4;return}}while(0);c[i>>2]=3;return}}while(0);if((f|0)!=1){return}c[d+32>>2]=1;return}function Cn(b,d,e,f,g){b=b|0;d=d|0;e=e|0;f=f|0;g=g|0;if((c[d+8>>2]|0)==(b|0)){if((c[d+4>>2]|0)!=(e|0)){return}g=d+28|0;if((c[g>>2]|0)==1){return}c[g>>2]=f;return}if((c[d>>2]|0)!=(b|0)){return}do{if((c[d+16>>2]|0)!=(e|0)){b=d+20|0;if((c[b>>2]|0)==(e|0)){break}c[d+32>>2]=f;c[b>>2]=e;b=d+40|0;c[b>>2]=(c[b>>2]|0)+1;do{if((c[d+36>>2]|0)==1){if((c[d+24>>2]|0)!=2){break}a[d+54|0]=1}}while(0);c[d+44>>2]=4;return}}while(0);if((f|0)!=1){return}c[d+32>>2]=1;return}function Dn(b,d,e,f,g,h){b=b|0;d=d|0;e=e|0;f=f|0;g=g|0;h=h|0;var i=0,j=0;if((b|0)!=(c[d+8>>2]|0)){i=c[b+8>>2]|0;sb[c[(c[i>>2]|0)+20>>2]&31](i,d,e,f,g,h);return}a[d+53|0]=1;if((c[d+4>>2]|0)!=(f|0)){return}a[d+52|0]=1;f=d+16|0;h=c[f>>2]|0;if((h|0)==0){c[f>>2]=e;c[d+24>>2]=g;c[d+36>>2]=1;if(!((c[d+48>>2]|0)==1&(g|0)==1)){return}a[d+54|0]=1;return}if((h|0)!=(e|0)){e=d+36|0;c[e>>2]=(c[e>>2]|0)+1;a[d+54|0]=1;return}e=d+24|0;h=c[e>>2]|0;if((h|0)==2){c[e>>2]=g;j=g}else{j=h}if(!((c[d+48>>2]|0)==1&(j|0)==1)){return}a[d+54|0]=1;return}function En(b,d,e,f,g,h){b=b|0;d=d|0;e=e|0;f=f|0;g=g|0;h=h|0;var i=0;if((c[d+8>>2]|0)!=(b|0)){return}a[d+53|0]=1;if((c[d+4>>2]|0)!=(f|0)){return}a[d+52|0]=1;f=d+16|0;b=c[f>>2]|0;if((b|0)==0){c[f>>2]=e;c[d+24>>2]=g;c[d+36>>2]=1;if(!((c[d+48>>2]|0)==1&(g|0)==1)){return}a[d+54|0]=1;return}if((b|0)!=(e|0)){e=d+36|0;c[e>>2]=(c[e>>2]|0)+1;a[d+54|0]=1;return}e=d+24|0;b=c[e>>2]|0;if((b|0)==2){c[e>>2]=g;i=g}else{i=b}if(!((c[d+48>>2]|0)==1&(i|0)==1)){return}a[d+54|0]=1;return}function Fn(a){a=a|0;var b=0,d=0,e=0,f=0,g=0,h=0,i=0,j=0,k=0,l=0,m=0,n=0,o=0,p=0,q=0,r=0,s=0,t=0,u=0,v=0,w=0,x=0,y=0,z=0,A=0,B=0,C=0,D=0,E=0,F=0,G=0,H=0,I=0,J=0,K=0,L=0,M=0,N=0,O=0,P=0,Q=0,R=0,S=0,T=0,U=0,V=0,W=0,X=0,Y=0,Z=0,_=0,$=0,aa=0,ba=0,ca=0,da=0,ea=0,fa=0,ga=0,ha=0,ia=0,ja=0,ka=0,la=0,ma=0,na=0,oa=0,pa=0,qa=0,ra=0,sa=0,ta=0,ua=0,va=0,wa=0,xa=0,ya=0,za=0,Aa=0,Ba=0,Ca=0,Da=0,Fa=0,Ga=0,Ha=0;do{if(a>>>0<245>>>0){if(a>>>0<11>>>0){b=16}else{b=a+11&-8}d=b>>>3;e=c[2152]|0;f=e>>>(d>>>0);if((f&3|0)!=0){g=(f&1^1)+d|0;h=g<<1;i=8648+(h<<2)|0;j=8648+(h+2<<2)|0;h=c[j>>2]|0;k=h+8|0;l=c[k>>2]|0;do{if((i|0)==(l|0)){c[2152]=e&~(1<<g)}else{if(l>>>0<(c[2156]|0)>>>0){Ea();return 0}m=l+12|0;if((c[m>>2]|0)==(h|0)){c[m>>2]=i;c[j>>2]=l;break}else{Ea();return 0}}}while(0);l=g<<3;c[h+4>>2]=l|3;j=h+(l|4)|0;c[j>>2]=c[j>>2]|1;n=k;return n|0}if(!(b>>>0>(c[2154]|0)>>>0)){o=b;break}if((f|0)!=0){j=2<<d;l=f<<d&(j|-j);j=(l&-l)-1|0;l=j>>>12&16;i=j>>>(l>>>0);j=i>>>5&8;m=i>>>(j>>>0);i=m>>>2&4;p=m>>>(i>>>0);m=p>>>1&2;q=p>>>(m>>>0);p=q>>>1&1;r=(j|l|i|m|p)+(q>>>(p>>>0))|0;p=r<<1;q=8648+(p<<2)|0;m=8648+(p+2<<2)|0;p=c[m>>2]|0;i=p+8|0;l=c[i>>2]|0;do{if((q|0)==(l|0)){c[2152]=e&~(1<<r)}else{if(l>>>0<(c[2156]|0)>>>0){Ea();return 0}j=l+12|0;if((c[j>>2]|0)==(p|0)){c[j>>2]=q;c[m>>2]=l;break}else{Ea();return 0}}}while(0);l=r<<3;m=l-b|0;c[p+4>>2]=b|3;q=p;e=q+b|0;c[q+(b|4)>>2]=m|1;c[q+l>>2]=m;l=c[2154]|0;if((l|0)!=0){q=c[2157]|0;d=l>>>3;l=d<<1;f=8648+(l<<2)|0;k=c[2152]|0;h=1<<d;do{if((k&h|0)==0){c[2152]=k|h;s=f;t=8648+(l+2<<2)|0}else{d=8648+(l+2<<2)|0;g=c[d>>2]|0;if(!(g>>>0<(c[2156]|0)>>>0)){s=g;t=d;break}Ea();return 0}}while(0);c[t>>2]=q;c[s+12>>2]=q;c[q+8>>2]=s;c[q+12>>2]=f}c[2154]=m;c[2157]=e;n=i;return n|0}l=c[2153]|0;if((l|0)==0){o=b;break}h=(l&-l)-1|0;l=h>>>12&16;k=h>>>(l>>>0);h=k>>>5&8;p=k>>>(h>>>0);k=p>>>2&4;r=p>>>(k>>>0);p=r>>>1&2;d=r>>>(p>>>0);r=d>>>1&1;g=c[8912+((h|l|k|p|r)+(d>>>(r>>>0))<<2)>>2]|0;r=g;d=g;p=(c[g+4>>2]&-8)-b|0;while(1){g=c[r+16>>2]|0;if((g|0)==0){k=c[r+20>>2]|0;if((k|0)==0){break}else{u=k}}else{u=g}g=(c[u+4>>2]&-8)-b|0;k=g>>>0<p>>>0;r=u;d=k?u:d;p=k?g:p}r=d;i=c[2156]|0;if(r>>>0<i>>>0){Ea();return 0}e=r+b|0;m=e;if(!(r>>>0<e>>>0)){Ea();return 0}e=c[d+24>>2]|0;f=c[d+12>>2]|0;do{if((f|0)==(d|0)){q=d+20|0;g=c[q>>2]|0;if((g|0)==0){k=d+16|0;l=c[k>>2]|0;if((l|0)==0){v=0;break}else{w=l;x=k}}else{w=g;x=q}while(1){q=w+20|0;g=c[q>>2]|0;if((g|0)!=0){w=g;x=q;continue}q=w+16|0;g=c[q>>2]|0;if((g|0)==0){break}else{w=g;x=q}}if(x>>>0<i>>>0){Ea();return 0}else{c[x>>2]=0;v=w;break}}else{q=c[d+8>>2]|0;if(q>>>0<i>>>0){Ea();return 0}g=q+12|0;if((c[g>>2]|0)!=(d|0)){Ea();return 0}k=f+8|0;if((c[k>>2]|0)==(d|0)){c[g>>2]=f;c[k>>2]=q;v=f;break}else{Ea();return 0}}}while(0);a:do{if((e|0)!=0){f=c[d+28>>2]|0;i=8912+(f<<2)|0;do{if((d|0)==(c[i>>2]|0)){c[i>>2]=v;if((v|0)!=0){break}c[2153]=c[2153]&~(1<<f);break a}else{if(e>>>0<(c[2156]|0)>>>0){Ea();return 0}q=e+16|0;if((c[q>>2]|0)==(d|0)){c[q>>2]=v}else{c[e+20>>2]=v}if((v|0)==0){break a}}}while(0);if(v>>>0<(c[2156]|0)>>>0){Ea();return 0}c[v+24>>2]=e;f=c[d+16>>2]|0;do{if((f|0)!=0){if(f>>>0<(c[2156]|0)>>>0){Ea();return 0}else{c[v+16>>2]=f;c[f+24>>2]=v;break}}}while(0);f=c[d+20>>2]|0;if((f|0)==0){break}if(f>>>0<(c[2156]|0)>>>0){Ea();return 0}else{c[v+20>>2]=f;c[f+24>>2]=v;break}}}while(0);if(p>>>0<16>>>0){e=p+b|0;c[d+4>>2]=e|3;f=r+(e+4)|0;c[f>>2]=c[f>>2]|1}else{c[d+4>>2]=b|3;c[r+(b|4)>>2]=p|1;c[r+(p+b)>>2]=p;f=c[2154]|0;if((f|0)!=0){e=c[2157]|0;i=f>>>3;f=i<<1;q=8648+(f<<2)|0;k=c[2152]|0;g=1<<i;do{if((k&g|0)==0){c[2152]=k|g;y=q;z=8648+(f+2<<2)|0}else{i=8648+(f+2<<2)|0;l=c[i>>2]|0;if(!(l>>>0<(c[2156]|0)>>>0)){y=l;z=i;break}Ea();return 0}}while(0);c[z>>2]=e;c[y+12>>2]=e;c[e+8>>2]=y;c[e+12>>2]=q}c[2154]=p;c[2157]=m}f=d+8|0;if((f|0)==0){o=b;break}else{n=f}return n|0}else{if(a>>>0>4294967231>>>0){o=-1;break}f=a+11|0;g=f&-8;k=c[2153]|0;if((k|0)==0){o=g;break}r=-g|0;i=f>>>8;do{if((i|0)==0){A=0}else{if(g>>>0>16777215>>>0){A=31;break}f=(i+1048320|0)>>>16&8;l=i<<f;h=(l+520192|0)>>>16&4;j=l<<h;l=(j+245760|0)>>>16&2;B=14-(h|f|l)+(j<<l>>>15)|0;A=g>>>((B+7|0)>>>0)&1|B<<1}}while(0);i=c[8912+(A<<2)>>2]|0;b:do{if((i|0)==0){C=0;D=r;E=0}else{if((A|0)==31){F=0}else{F=25-(A>>>1)|0}d=0;m=r;p=i;q=g<<F;e=0;while(1){B=c[p+4>>2]&-8;l=B-g|0;if(l>>>0<m>>>0){if((B|0)==(g|0)){C=p;D=l;E=p;break b}else{G=p;H=l}}else{G=d;H=m}l=c[p+20>>2]|0;B=c[p+16+(q>>>31<<2)>>2]|0;j=(l|0)==0|(l|0)==(B|0)?e:l;if((B|0)==0){C=G;D=H;E=j;break}else{d=G;m=H;p=B;q=q<<1;e=j}}}}while(0);if((E|0)==0&(C|0)==0){i=2<<A;r=k&(i|-i);if((r|0)==0){o=g;break}i=(r&-r)-1|0;r=i>>>12&16;e=i>>>(r>>>0);i=e>>>5&8;q=e>>>(i>>>0);e=q>>>2&4;p=q>>>(e>>>0);q=p>>>1&2;m=p>>>(q>>>0);p=m>>>1&1;I=c[8912+((i|r|e|q|p)+(m>>>(p>>>0))<<2)>>2]|0}else{I=E}if((I|0)==0){J=D;K=C}else{p=I;m=D;q=C;while(1){e=(c[p+4>>2]&-8)-g|0;r=e>>>0<m>>>0;i=r?e:m;e=r?p:q;r=c[p+16>>2]|0;if((r|0)!=0){p=r;m=i;q=e;continue}r=c[p+20>>2]|0;if((r|0)==0){J=i;K=e;break}else{p=r;m=i;q=e}}}if((K|0)==0){o=g;break}if(!(J>>>0<((c[2154]|0)-g|0)>>>0)){o=g;break}q=K;m=c[2156]|0;if(q>>>0<m>>>0){Ea();return 0}p=q+g|0;k=p;if(!(q>>>0<p>>>0)){Ea();return 0}e=c[K+24>>2]|0;i=c[K+12>>2]|0;do{if((i|0)==(K|0)){r=K+20|0;d=c[r>>2]|0;if((d|0)==0){j=K+16|0;B=c[j>>2]|0;if((B|0)==0){L=0;break}else{M=B;N=j}}else{M=d;N=r}while(1){r=M+20|0;d=c[r>>2]|0;if((d|0)!=0){M=d;N=r;continue}r=M+16|0;d=c[r>>2]|0;if((d|0)==0){break}else{M=d;N=r}}if(N>>>0<m>>>0){Ea();return 0}else{c[N>>2]=0;L=M;break}}else{r=c[K+8>>2]|0;if(r>>>0<m>>>0){Ea();return 0}d=r+12|0;if((c[d>>2]|0)!=(K|0)){Ea();return 0}j=i+8|0;if((c[j>>2]|0)==(K|0)){c[d>>2]=i;c[j>>2]=r;L=i;break}else{Ea();return 0}}}while(0);c:do{if((e|0)!=0){i=c[K+28>>2]|0;m=8912+(i<<2)|0;do{if((K|0)==(c[m>>2]|0)){c[m>>2]=L;if((L|0)!=0){break}c[2153]=c[2153]&~(1<<i);break c}else{if(e>>>0<(c[2156]|0)>>>0){Ea();return 0}r=e+16|0;if((c[r>>2]|0)==(K|0)){c[r>>2]=L}else{c[e+20>>2]=L}if((L|0)==0){break c}}}while(0);if(L>>>0<(c[2156]|0)>>>0){Ea();return 0}c[L+24>>2]=e;i=c[K+16>>2]|0;do{if((i|0)!=0){if(i>>>0<(c[2156]|0)>>>0){Ea();return 0}else{c[L+16>>2]=i;c[i+24>>2]=L;break}}}while(0);i=c[K+20>>2]|0;if((i|0)==0){break}if(i>>>0<(c[2156]|0)>>>0){Ea();return 0}else{c[L+20>>2]=i;c[i+24>>2]=L;break}}}while(0);do{if(J>>>0<16>>>0){e=J+g|0;c[K+4>>2]=e|3;i=q+(e+4)|0;c[i>>2]=c[i>>2]|1}else{c[K+4>>2]=g|3;c[q+(g|4)>>2]=J|1;c[q+(J+g)>>2]=J;i=J>>>3;if(J>>>0<256>>>0){e=i<<1;m=8648+(e<<2)|0;r=c[2152]|0;j=1<<i;do{if((r&j|0)==0){c[2152]=r|j;O=m;P=8648+(e+2<<2)|0}else{i=8648+(e+2<<2)|0;d=c[i>>2]|0;if(!(d>>>0<(c[2156]|0)>>>0)){O=d;P=i;break}Ea();return 0}}while(0);c[P>>2]=k;c[O+12>>2]=k;c[q+(g+8)>>2]=O;c[q+(g+12)>>2]=m;break}e=p;j=J>>>8;do{if((j|0)==0){Q=0}else{if(J>>>0>16777215>>>0){Q=31;break}r=(j+1048320|0)>>>16&8;i=j<<r;d=(i+520192|0)>>>16&4;B=i<<d;i=(B+245760|0)>>>16&2;l=14-(d|r|i)+(B<<i>>>15)|0;Q=J>>>((l+7|0)>>>0)&1|l<<1}}while(0);j=8912+(Q<<2)|0;c[q+(g+28)>>2]=Q;c[q+(g+20)>>2]=0;c[q+(g+16)>>2]=0;m=c[2153]|0;l=1<<Q;if((m&l|0)==0){c[2153]=m|l;c[j>>2]=e;c[q+(g+24)>>2]=j;c[q+(g+12)>>2]=e;c[q+(g+8)>>2]=e;break}if((Q|0)==31){R=0}else{R=25-(Q>>>1)|0}l=J<<R;m=c[j>>2]|0;while(1){if((c[m+4>>2]&-8|0)==(J|0)){break}S=m+16+(l>>>31<<2)|0;j=c[S>>2]|0;if((j|0)==0){T=151;break}else{l=l<<1;m=j}}if((T|0)==151){if(S>>>0<(c[2156]|0)>>>0){Ea();return 0}else{c[S>>2]=e;c[q+(g+24)>>2]=m;c[q+(g+12)>>2]=e;c[q+(g+8)>>2]=e;break}}l=m+8|0;j=c[l>>2]|0;i=c[2156]|0;if(m>>>0<i>>>0){Ea();return 0}if(j>>>0<i>>>0){Ea();return 0}else{c[j+12>>2]=e;c[l>>2]=e;c[q+(g+8)>>2]=j;c[q+(g+12)>>2]=m;c[q+(g+24)>>2]=0;break}}}while(0);q=K+8|0;if((q|0)==0){o=g;break}else{n=q}return n|0}}while(0);K=c[2154]|0;if(!(o>>>0>K>>>0)){S=K-o|0;J=c[2157]|0;if(S>>>0>15>>>0){R=J;c[2157]=R+o;c[2154]=S;c[R+(o+4)>>2]=S|1;c[R+K>>2]=S;c[J+4>>2]=o|3}else{c[2154]=0;c[2157]=0;c[J+4>>2]=K|3;S=J+(K+4)|0;c[S>>2]=c[S>>2]|1}n=J+8|0;return n|0}J=c[2155]|0;if(o>>>0<J>>>0){S=J-o|0;c[2155]=S;J=c[2158]|0;K=J;c[2158]=K+o;c[K+(o+4)>>2]=S|1;c[J+4>>2]=o|3;n=J+8|0;return n|0}do{if((c[2126]|0)==0){J=Ka(30)|0;if((J-1&J|0)==0){c[2128]=J;c[2127]=J;c[2129]=-1;c[2130]=-1;c[2131]=0;c[2263]=0;c[2126]=(Ra(0)|0)&-16^1431655768;break}else{Ea();return 0}}}while(0);J=o+48|0;S=c[2128]|0;K=o+47|0;R=S+K|0;Q=-S|0;S=R&Q;if(!(S>>>0>o>>>0)){n=0;return n|0}O=c[2262]|0;do{if((O|0)!=0){P=c[2260]|0;L=P+S|0;if(L>>>0<=P>>>0|L>>>0>O>>>0){n=0}else{break}return n|0}}while(0);d:do{if((c[2263]&4|0)==0){O=c[2158]|0;e:do{if((O|0)==0){T=181}else{L=O;P=9056;while(1){U=P|0;M=c[U>>2]|0;if(!(M>>>0>L>>>0)){V=P+4|0;if((M+(c[V>>2]|0)|0)>>>0>L>>>0){break}}M=c[P+8>>2]|0;if((M|0)==0){T=181;break e}else{P=M}}if((P|0)==0){T=181;break}L=R-(c[2155]|0)&Q;if(!(L>>>0<2147483647>>>0)){W=0;break}m=_a(L|0)|0;e=(m|0)==((c[U>>2]|0)+(c[V>>2]|0)|0);X=e?m:-1;Y=e?L:0;Z=m;_=L;T=190}}while(0);do{if((T|0)==181){O=_a(0)|0;if((O|0)==-1){W=0;break}g=O;L=c[2127]|0;m=L-1|0;if((m&g|0)==0){$=S}else{$=S-g+(m+g&-L)|0}L=c[2260]|0;g=L+$|0;if(!($>>>0>o>>>0&$>>>0<2147483647>>>0)){W=0;break}m=c[2262]|0;if((m|0)!=0){if(g>>>0<=L>>>0|g>>>0>m>>>0){W=0;break}}m=_a($|0)|0;g=(m|0)==(O|0);X=g?O:-1;Y=g?$:0;Z=m;_=$;T=190}}while(0);f:do{if((T|0)==190){m=-_|0;if(!((X|0)==-1)){aa=Y;ba=X;T=201;break d}do{if((Z|0)!=-1&_>>>0<2147483647>>>0&_>>>0<J>>>0){g=c[2128]|0;O=K-_+g&-g;if(!(O>>>0<2147483647>>>0)){ca=_;break}if((_a(O|0)|0)==-1){_a(m|0)|0;W=Y;break f}else{ca=O+_|0;break}}else{ca=_}}while(0);if((Z|0)==-1){W=Y}else{aa=ca;ba=Z;T=201;break d}}}while(0);c[2263]=c[2263]|4;da=W;T=198}else{da=0;T=198}}while(0);do{if((T|0)==198){if(!(S>>>0<2147483647>>>0)){break}W=_a(S|0)|0;Z=_a(0)|0;if(!((Z|0)!=-1&(W|0)!=-1&W>>>0<Z>>>0)){break}ca=Z-W|0;Z=ca>>>0>(o+40|0)>>>0;Y=Z?W:-1;if(!((Y|0)==-1)){aa=Z?ca:da;ba=Y;T=201}}}while(0);do{if((T|0)==201){da=(c[2260]|0)+aa|0;c[2260]=da;if(da>>>0>(c[2261]|0)>>>0){c[2261]=da}da=c[2158]|0;g:do{if((da|0)==0){S=c[2156]|0;if((S|0)==0|ba>>>0<S>>>0){c[2156]=ba}c[2264]=ba;c[2265]=aa;c[2267]=0;c[2161]=c[2126];c[2160]=-1;S=0;do{Y=S<<1;ca=8648+(Y<<2)|0;c[8648+(Y+3<<2)>>2]=ca;c[8648+(Y+2<<2)>>2]=ca;S=S+1|0;}while(S>>>0<32>>>0);S=ba+8|0;if((S&7|0)==0){ea=0}else{ea=-S&7}S=aa-40-ea|0;c[2158]=ba+ea;c[2155]=S;c[ba+(ea+4)>>2]=S|1;c[ba+(aa-36)>>2]=40;c[2159]=c[2130]}else{S=9056;while(1){fa=c[S>>2]|0;ga=S+4|0;ha=c[ga>>2]|0;if((ba|0)==(fa+ha|0)){T=213;break}ca=c[S+8>>2]|0;if((ca|0)==0){break}else{S=ca}}do{if((T|0)==213){if((c[S+12>>2]&8|0)!=0){break}ca=da;if(!(ca>>>0>=fa>>>0&ca>>>0<ba>>>0)){break}c[ga>>2]=ha+aa;Y=(c[2155]|0)+aa|0;Z=da+8|0;if((Z&7|0)==0){ia=0}else{ia=-Z&7}Z=Y-ia|0;c[2158]=ca+ia;c[2155]=Z;c[ca+(ia+4)>>2]=Z|1;c[ca+(Y+4)>>2]=40;c[2159]=c[2130];break g}}while(0);if(ba>>>0<(c[2156]|0)>>>0){c[2156]=ba}S=ba+aa|0;Y=9056;while(1){ja=Y|0;if((c[ja>>2]|0)==(S|0)){T=223;break}ca=c[Y+8>>2]|0;if((ca|0)==0){break}else{Y=ca}}do{if((T|0)==223){if((c[Y+12>>2]&8|0)!=0){break}c[ja>>2]=ba;S=Y+4|0;c[S>>2]=(c[S>>2]|0)+aa;S=ba+8|0;if((S&7|0)==0){ka=0}else{ka=-S&7}S=ba+(aa+8)|0;if((S&7|0)==0){la=0}else{la=-S&7}S=ba+(la+aa)|0;ca=S;Z=ka+o|0;W=ba+Z|0;_=W;K=S-(ba+ka)-o|0;c[ba+(ka+4)>>2]=o|3;do{if((ca|0)==(c[2158]|0)){J=(c[2155]|0)+K|0;c[2155]=J;c[2158]=_;c[ba+(Z+4)>>2]=J|1}else{if((ca|0)==(c[2157]|0)){J=(c[2154]|0)+K|0;c[2154]=J;c[2157]=_;c[ba+(Z+4)>>2]=J|1;c[ba+(J+Z)>>2]=J;break}J=aa+4|0;X=c[ba+(J+la)>>2]|0;if((X&3|0)==1){$=X&-8;V=X>>>3;h:do{if(X>>>0<256>>>0){U=c[ba+((la|8)+aa)>>2]|0;Q=c[ba+(aa+12+la)>>2]|0;R=8648+(V<<1<<2)|0;do{if((U|0)!=(R|0)){if(U>>>0<(c[2156]|0)>>>0){Ea();return 0}if((c[U+12>>2]|0)==(ca|0)){break}Ea();return 0}}while(0);if((Q|0)==(U|0)){c[2152]=c[2152]&~(1<<V);break}do{if((Q|0)==(R|0)){ma=Q+8|0}else{if(Q>>>0<(c[2156]|0)>>>0){Ea();return 0}m=Q+8|0;if((c[m>>2]|0)==(ca|0)){ma=m;break}Ea();return 0}}while(0);c[U+12>>2]=Q;c[ma>>2]=U}else{R=S;m=c[ba+((la|24)+aa)>>2]|0;P=c[ba+(aa+12+la)>>2]|0;do{if((P|0)==(R|0)){O=la|16;g=ba+(J+O)|0;L=c[g>>2]|0;if((L|0)==0){e=ba+(O+aa)|0;O=c[e>>2]|0;if((O|0)==0){na=0;break}else{oa=O;pa=e}}else{oa=L;pa=g}while(1){g=oa+20|0;L=c[g>>2]|0;if((L|0)!=0){oa=L;pa=g;continue}g=oa+16|0;L=c[g>>2]|0;if((L|0)==0){break}else{oa=L;pa=g}}if(pa>>>0<(c[2156]|0)>>>0){Ea();return 0}else{c[pa>>2]=0;na=oa;break}}else{g=c[ba+((la|8)+aa)>>2]|0;if(g>>>0<(c[2156]|0)>>>0){Ea();return 0}L=g+12|0;if((c[L>>2]|0)!=(R|0)){Ea();return 0}e=P+8|0;if((c[e>>2]|0)==(R|0)){c[L>>2]=P;c[e>>2]=g;na=P;break}else{Ea();return 0}}}while(0);if((m|0)==0){break}P=c[ba+(aa+28+la)>>2]|0;U=8912+(P<<2)|0;do{if((R|0)==(c[U>>2]|0)){c[U>>2]=na;if((na|0)!=0){break}c[2153]=c[2153]&~(1<<P);break h}else{if(m>>>0<(c[2156]|0)>>>0){Ea();return 0}Q=m+16|0;if((c[Q>>2]|0)==(R|0)){c[Q>>2]=na}else{c[m+20>>2]=na}if((na|0)==0){break h}}}while(0);if(na>>>0<(c[2156]|0)>>>0){Ea();return 0}c[na+24>>2]=m;R=la|16;P=c[ba+(R+aa)>>2]|0;do{if((P|0)!=0){if(P>>>0<(c[2156]|0)>>>0){Ea();return 0}else{c[na+16>>2]=P;c[P+24>>2]=na;break}}}while(0);P=c[ba+(J+R)>>2]|0;if((P|0)==0){break}if(P>>>0<(c[2156]|0)>>>0){Ea();return 0}else{c[na+20>>2]=P;c[P+24>>2]=na;break}}}while(0);qa=ba+(($|la)+aa)|0;ra=$+K|0}else{qa=ca;ra=K}J=qa+4|0;c[J>>2]=c[J>>2]&-2;c[ba+(Z+4)>>2]=ra|1;c[ba+(ra+Z)>>2]=ra;J=ra>>>3;if(ra>>>0<256>>>0){V=J<<1;X=8648+(V<<2)|0;P=c[2152]|0;m=1<<J;do{if((P&m|0)==0){c[2152]=P|m;sa=X;ta=8648+(V+2<<2)|0}else{J=8648+(V+2<<2)|0;U=c[J>>2]|0;if(!(U>>>0<(c[2156]|0)>>>0)){sa=U;ta=J;break}Ea();return 0}}while(0);c[ta>>2]=_;c[sa+12>>2]=_;c[ba+(Z+8)>>2]=sa;c[ba+(Z+12)>>2]=X;break}V=W;m=ra>>>8;do{if((m|0)==0){ua=0}else{if(ra>>>0>16777215>>>0){ua=31;break}P=(m+1048320|0)>>>16&8;$=m<<P;J=($+520192|0)>>>16&4;U=$<<J;$=(U+245760|0)>>>16&2;Q=14-(J|P|$)+(U<<$>>>15)|0;ua=ra>>>((Q+7|0)>>>0)&1|Q<<1}}while(0);m=8912+(ua<<2)|0;c[ba+(Z+28)>>2]=ua;c[ba+(Z+20)>>2]=0;c[ba+(Z+16)>>2]=0;X=c[2153]|0;Q=1<<ua;if((X&Q|0)==0){c[2153]=X|Q;c[m>>2]=V;c[ba+(Z+24)>>2]=m;c[ba+(Z+12)>>2]=V;c[ba+(Z+8)>>2]=V;break}if((ua|0)==31){va=0}else{va=25-(ua>>>1)|0}Q=ra<<va;X=c[m>>2]|0;while(1){if((c[X+4>>2]&-8|0)==(ra|0)){break}wa=X+16+(Q>>>31<<2)|0;m=c[wa>>2]|0;if((m|0)==0){T=296;break}else{Q=Q<<1;X=m}}if((T|0)==296){if(wa>>>0<(c[2156]|0)>>>0){Ea();return 0}else{c[wa>>2]=V;c[ba+(Z+24)>>2]=X;c[ba+(Z+12)>>2]=V;c[ba+(Z+8)>>2]=V;break}}Q=X+8|0;m=c[Q>>2]|0;$=c[2156]|0;if(X>>>0<$>>>0){Ea();return 0}if(m>>>0<$>>>0){Ea();return 0}else{c[m+12>>2]=V;c[Q>>2]=V;c[ba+(Z+8)>>2]=m;c[ba+(Z+12)>>2]=X;c[ba+(Z+24)>>2]=0;break}}}while(0);n=ba+(ka|8)|0;return n|0}}while(0);Y=da;Z=9056;while(1){xa=c[Z>>2]|0;if(!(xa>>>0>Y>>>0)){ya=c[Z+4>>2]|0;za=xa+ya|0;if(za>>>0>Y>>>0){break}}Z=c[Z+8>>2]|0}Z=xa+(ya-39)|0;if((Z&7|0)==0){Aa=0}else{Aa=-Z&7}Z=xa+(ya-47+Aa)|0;W=Z>>>0<(da+16|0)>>>0?Y:Z;Z=W+8|0;_=ba+8|0;if((_&7|0)==0){Ba=0}else{Ba=-_&7}_=aa-40-Ba|0;c[2158]=ba+Ba;c[2155]=_;c[ba+(Ba+4)>>2]=_|1;c[ba+(aa-36)>>2]=40;c[2159]=c[2130];c[W+4>>2]=27;c[Z>>2]=c[2264];c[Z+4>>2]=c[2265];c[Z+8>>2]=c[2266];c[Z+12>>2]=c[2267];c[2264]=ba;c[2265]=aa;c[2267]=0;c[2266]=Z;Z=W+28|0;c[Z>>2]=7;if((W+32|0)>>>0<za>>>0){_=Z;while(1){Z=_+4|0;c[Z>>2]=7;if((_+8|0)>>>0<za>>>0){_=Z}else{break}}}if((W|0)==(Y|0)){break}_=W-da|0;Z=Y+(_+4)|0;c[Z>>2]=c[Z>>2]&-2;c[da+4>>2]=_|1;c[Y+_>>2]=_;Z=_>>>3;if(_>>>0<256>>>0){K=Z<<1;ca=8648+(K<<2)|0;S=c[2152]|0;m=1<<Z;do{if((S&m|0)==0){c[2152]=S|m;Ca=ca;Da=8648+(K+2<<2)|0}else{Z=8648+(K+2<<2)|0;Q=c[Z>>2]|0;if(!(Q>>>0<(c[2156]|0)>>>0)){Ca=Q;Da=Z;break}Ea();return 0}}while(0);c[Da>>2]=da;c[Ca+12>>2]=da;c[da+8>>2]=Ca;c[da+12>>2]=ca;break}K=da;m=_>>>8;do{if((m|0)==0){Fa=0}else{if(_>>>0>16777215>>>0){Fa=31;break}S=(m+1048320|0)>>>16&8;Y=m<<S;W=(Y+520192|0)>>>16&4;Z=Y<<W;Y=(Z+245760|0)>>>16&2;Q=14-(W|S|Y)+(Z<<Y>>>15)|0;Fa=_>>>((Q+7|0)>>>0)&1|Q<<1}}while(0);m=8912+(Fa<<2)|0;c[da+28>>2]=Fa;c[da+20>>2]=0;c[da+16>>2]=0;ca=c[2153]|0;Q=1<<Fa;if((ca&Q|0)==0){c[2153]=ca|Q;c[m>>2]=K;c[da+24>>2]=m;c[da+12>>2]=da;c[da+8>>2]=da;break}if((Fa|0)==31){Ga=0}else{Ga=25-(Fa>>>1)|0}Q=_<<Ga;ca=c[m>>2]|0;while(1){if((c[ca+4>>2]&-8|0)==(_|0)){break}Ha=ca+16+(Q>>>31<<2)|0;m=c[Ha>>2]|0;if((m|0)==0){T=331;break}else{Q=Q<<1;ca=m}}if((T|0)==331){if(Ha>>>0<(c[2156]|0)>>>0){Ea();return 0}else{c[Ha>>2]=K;c[da+24>>2]=ca;c[da+12>>2]=da;c[da+8>>2]=da;break}}Q=ca+8|0;_=c[Q>>2]|0;m=c[2156]|0;if(ca>>>0<m>>>0){Ea();return 0}if(_>>>0<m>>>0){Ea();return 0}else{c[_+12>>2]=K;c[Q>>2]=K;c[da+8>>2]=_;c[da+12>>2]=ca;c[da+24>>2]=0;break}}}while(0);da=c[2155]|0;if(!(da>>>0>o>>>0)){break}_=da-o|0;c[2155]=_;da=c[2158]|0;Q=da;c[2158]=Q+o;c[Q+(o+4)>>2]=_|1;c[da+4>>2]=o|3;n=da+8|0;return n|0}}while(0);c[(ab()|0)>>2]=12;n=0;return n|0}function Gn(a){a=a|0;var b=0,d=0,e=0,f=0,g=0,h=0,i=0,j=0,k=0,l=0,m=0,n=0,o=0,p=0,q=0,r=0,s=0,t=0,u=0,v=0,w=0,x=0,y=0,z=0,A=0,B=0,C=0,D=0,E=0,F=0,G=0,H=0,I=0,J=0,K=0,L=0,M=0,N=0,O=0;if((a|0)==0){return}b=a-8|0;d=b;e=c[2156]|0;if(b>>>0<e>>>0){Ea()}f=c[a-4>>2]|0;g=f&3;if((g|0)==1){Ea()}h=f&-8;i=a+(h-8)|0;j=i;a:do{if((f&1|0)==0){k=c[b>>2]|0;if((g|0)==0){return}l=-8-k|0;m=a+l|0;n=m;o=k+h|0;if(m>>>0<e>>>0){Ea()}if((n|0)==(c[2157]|0)){p=a+(h-4)|0;if((c[p>>2]&3|0)!=3){q=n;r=o;break}c[2154]=o;c[p>>2]=c[p>>2]&-2;c[a+(l+4)>>2]=o|1;c[i>>2]=o;return}p=k>>>3;if(k>>>0<256>>>0){k=c[a+(l+8)>>2]|0;s=c[a+(l+12)>>2]|0;t=8648+(p<<1<<2)|0;do{if((k|0)!=(t|0)){if(k>>>0<e>>>0){Ea()}if((c[k+12>>2]|0)==(n|0)){break}Ea()}}while(0);if((s|0)==(k|0)){c[2152]=c[2152]&~(1<<p);q=n;r=o;break}do{if((s|0)==(t|0)){u=s+8|0}else{if(s>>>0<e>>>0){Ea()}v=s+8|0;if((c[v>>2]|0)==(n|0)){u=v;break}Ea()}}while(0);c[k+12>>2]=s;c[u>>2]=k;q=n;r=o;break}t=m;p=c[a+(l+24)>>2]|0;v=c[a+(l+12)>>2]|0;do{if((v|0)==(t|0)){w=a+(l+20)|0;x=c[w>>2]|0;if((x|0)==0){y=a+(l+16)|0;z=c[y>>2]|0;if((z|0)==0){A=0;break}else{B=z;C=y}}else{B=x;C=w}while(1){w=B+20|0;x=c[w>>2]|0;if((x|0)!=0){B=x;C=w;continue}w=B+16|0;x=c[w>>2]|0;if((x|0)==0){break}else{B=x;C=w}}if(C>>>0<e>>>0){Ea()}else{c[C>>2]=0;A=B;break}}else{w=c[a+(l+8)>>2]|0;if(w>>>0<e>>>0){Ea()}x=w+12|0;if((c[x>>2]|0)!=(t|0)){Ea()}y=v+8|0;if((c[y>>2]|0)==(t|0)){c[x>>2]=v;c[y>>2]=w;A=v;break}else{Ea()}}}while(0);if((p|0)==0){q=n;r=o;break}v=c[a+(l+28)>>2]|0;m=8912+(v<<2)|0;do{if((t|0)==(c[m>>2]|0)){c[m>>2]=A;if((A|0)!=0){break}c[2153]=c[2153]&~(1<<v);q=n;r=o;break a}else{if(p>>>0<(c[2156]|0)>>>0){Ea()}k=p+16|0;if((c[k>>2]|0)==(t|0)){c[k>>2]=A}else{c[p+20>>2]=A}if((A|0)==0){q=n;r=o;break a}}}while(0);if(A>>>0<(c[2156]|0)>>>0){Ea()}c[A+24>>2]=p;t=c[a+(l+16)>>2]|0;do{if((t|0)!=0){if(t>>>0<(c[2156]|0)>>>0){Ea()}else{c[A+16>>2]=t;c[t+24>>2]=A;break}}}while(0);t=c[a+(l+20)>>2]|0;if((t|0)==0){q=n;r=o;break}if(t>>>0<(c[2156]|0)>>>0){Ea()}else{c[A+20>>2]=t;c[t+24>>2]=A;q=n;r=o;break}}else{q=d;r=h}}while(0);d=q;if(!(d>>>0<i>>>0)){Ea()}A=a+(h-4)|0;e=c[A>>2]|0;if((e&1|0)==0){Ea()}do{if((e&2|0)==0){if((j|0)==(c[2158]|0)){B=(c[2155]|0)+r|0;c[2155]=B;c[2158]=q;c[q+4>>2]=B|1;if((q|0)!=(c[2157]|0)){return}c[2157]=0;c[2154]=0;return}if((j|0)==(c[2157]|0)){B=(c[2154]|0)+r|0;c[2154]=B;c[2157]=q;c[q+4>>2]=B|1;c[d+B>>2]=B;return}B=(e&-8)+r|0;C=e>>>3;b:do{if(e>>>0<256>>>0){u=c[a+h>>2]|0;g=c[a+(h|4)>>2]|0;b=8648+(C<<1<<2)|0;do{if((u|0)!=(b|0)){if(u>>>0<(c[2156]|0)>>>0){Ea()}if((c[u+12>>2]|0)==(j|0)){break}Ea()}}while(0);if((g|0)==(u|0)){c[2152]=c[2152]&~(1<<C);break}do{if((g|0)==(b|0)){D=g+8|0}else{if(g>>>0<(c[2156]|0)>>>0){Ea()}f=g+8|0;if((c[f>>2]|0)==(j|0)){D=f;break}Ea()}}while(0);c[u+12>>2]=g;c[D>>2]=u}else{b=i;f=c[a+(h+16)>>2]|0;t=c[a+(h|4)>>2]|0;do{if((t|0)==(b|0)){p=a+(h+12)|0;v=c[p>>2]|0;if((v|0)==0){m=a+(h+8)|0;k=c[m>>2]|0;if((k|0)==0){E=0;break}else{F=k;G=m}}else{F=v;G=p}while(1){p=F+20|0;v=c[p>>2]|0;if((v|0)!=0){F=v;G=p;continue}p=F+16|0;v=c[p>>2]|0;if((v|0)==0){break}else{F=v;G=p}}if(G>>>0<(c[2156]|0)>>>0){Ea()}else{c[G>>2]=0;E=F;break}}else{p=c[a+h>>2]|0;if(p>>>0<(c[2156]|0)>>>0){Ea()}v=p+12|0;if((c[v>>2]|0)!=(b|0)){Ea()}m=t+8|0;if((c[m>>2]|0)==(b|0)){c[v>>2]=t;c[m>>2]=p;E=t;break}else{Ea()}}}while(0);if((f|0)==0){break}t=c[a+(h+20)>>2]|0;u=8912+(t<<2)|0;do{if((b|0)==(c[u>>2]|0)){c[u>>2]=E;if((E|0)!=0){break}c[2153]=c[2153]&~(1<<t);break b}else{if(f>>>0<(c[2156]|0)>>>0){Ea()}g=f+16|0;if((c[g>>2]|0)==(b|0)){c[g>>2]=E}else{c[f+20>>2]=E}if((E|0)==0){break b}}}while(0);if(E>>>0<(c[2156]|0)>>>0){Ea()}c[E+24>>2]=f;b=c[a+(h+8)>>2]|0;do{if((b|0)!=0){if(b>>>0<(c[2156]|0)>>>0){Ea()}else{c[E+16>>2]=b;c[b+24>>2]=E;break}}}while(0);b=c[a+(h+12)>>2]|0;if((b|0)==0){break}if(b>>>0<(c[2156]|0)>>>0){Ea()}else{c[E+20>>2]=b;c[b+24>>2]=E;break}}}while(0);c[q+4>>2]=B|1;c[d+B>>2]=B;if((q|0)!=(c[2157]|0)){H=B;break}c[2154]=B;return}else{c[A>>2]=e&-2;c[q+4>>2]=r|1;c[d+r>>2]=r;H=r}}while(0);r=H>>>3;if(H>>>0<256>>>0){d=r<<1;e=8648+(d<<2)|0;A=c[2152]|0;E=1<<r;do{if((A&E|0)==0){c[2152]=A|E;I=e;J=8648+(d+2<<2)|0}else{r=8648+(d+2<<2)|0;h=c[r>>2]|0;if(!(h>>>0<(c[2156]|0)>>>0)){I=h;J=r;break}Ea()}}while(0);c[J>>2]=q;c[I+12>>2]=q;c[q+8>>2]=I;c[q+12>>2]=e;return}e=q;I=H>>>8;do{if((I|0)==0){K=0}else{if(H>>>0>16777215>>>0){K=31;break}J=(I+1048320|0)>>>16&8;d=I<<J;E=(d+520192|0)>>>16&4;A=d<<E;d=(A+245760|0)>>>16&2;r=14-(E|J|d)+(A<<d>>>15)|0;K=H>>>((r+7|0)>>>0)&1|r<<1}}while(0);I=8912+(K<<2)|0;c[q+28>>2]=K;c[q+20>>2]=0;c[q+16>>2]=0;r=c[2153]|0;d=1<<K;do{if((r&d|0)==0){c[2153]=r|d;c[I>>2]=e;c[q+24>>2]=I;c[q+12>>2]=q;c[q+8>>2]=q}else{if((K|0)==31){L=0}else{L=25-(K>>>1)|0}A=H<<L;J=c[I>>2]|0;while(1){if((c[J+4>>2]&-8|0)==(H|0)){break}M=J+16+(A>>>31<<2)|0;E=c[M>>2]|0;if((E|0)==0){N=129;break}else{A=A<<1;J=E}}if((N|0)==129){if(M>>>0<(c[2156]|0)>>>0){Ea()}else{c[M>>2]=e;c[q+24>>2]=J;c[q+12>>2]=q;c[q+8>>2]=q;break}}A=J+8|0;B=c[A>>2]|0;E=c[2156]|0;if(J>>>0<E>>>0){Ea()}if(B>>>0<E>>>0){Ea()}else{c[B+12>>2]=e;c[A>>2]=e;c[q+8>>2]=B;c[q+12>>2]=J;c[q+24>>2]=0;break}}}while(0);q=(c[2160]|0)-1|0;c[2160]=q;if((q|0)==0){O=9064}else{return}while(1){q=c[O>>2]|0;if((q|0)==0){break}else{O=q+8|0}}c[2160]=-1;return}function Hn(a){a=a|0;var b=0,d=0,e=0;b=(a|0)==0?1:a;while(1){d=Fn(b)|0;if((d|0)!=0){e=10;break}a=(D=c[2488]|0,c[2488]=D+0,D);if((a|0)==0){break}pb[a&3]()}if((e|0)==10){return d|0}d=Oa(4)|0;c[d>>2]=4480;Ca(d|0,7824,90);return 0}function In(a,b){a=a|0;b=b|0;return Hn(a)|0}function Jn(a){a=a|0;if((a|0)==0){return}Gn(a);return}function Kn(a){a=a|0;Jn(a);return}function Ln(a){a=a|0;return}function Mn(a){a=a|0;return 2256}function Nn(b,d,e){b=b|0;d=d|0;e=e|0;var f=0,g=0,h=0,i=0;f=b+e|0;if((e|0)>=20){d=d&255;g=b&3;h=d|d<<8|d<<16|d<<24;i=f&~3;if(g){g=b+4-g|0;while((b|0)<(g|0)){a[b]=d;b=b+1|0}}while((b|0)<(i|0)){c[b>>2]=h;b=b+4|0}}while((b|0)<(f|0)){a[b]=d;b=b+1|0}return b-e|0}function On(b,d,e){b=b|0;d=d|0;e=e|0;var f=0;if((e|0)>=4096)return Xa(b|0,d|0,e|0)|0;f=b|0;if((b&3)==(d&3)){while(b&3){if((e|0)==0)return f|0;a[b]=a[d]|0;b=b+1|0;d=d+1|0;e=e-1|0}while((e|0)>=4){c[b>>2]=c[d>>2];b=b+4|0;d=d+4|0;e=e-4|0}}while((e|0)>0){a[b]=a[d]|0;b=b+1|0;d=d+1|0;e=e-1|0}return f|0}function Pn(b,c,d){b=b|0;c=c|0;d=d|0;var e=0;if((c|0)<(b|0)&(b|0)<(c+d|0)){e=b;c=c+d|0;b=b+d|0;while((d|0)>0){b=b-1|0;c=c-1|0;d=d-1|0;a[b]=a[c]|0}b=e}else{On(b,c,d)|0}return b|0}function Qn(b){b=b|0;var c=0;c=b;while(a[c]|0){c=c+1|0}return c-b|0}function Rn(a,b,c,d){a=a|0;b=b|0;c=c|0;d=d|0;var e=0;e=a+c>>>0;return(F=b+d+(e>>>0<a>>>0|0)>>>0,e|0)|0}function Sn(a,b,c,d){a=a|0;b=b|0;c=c|0;d=d|0;var e=0;e=b-d>>>0;e=b-d-(c>>>0>a>>>0|0)>>>0;return(F=e,a-c>>>0|0)|0}function Tn(a,b,c){a=a|0;b=b|0;c=c|0;if((c|0)<32){F=b<<c|(a&(1<<c)-1<<32-c)>>>32-c;return a<<c}F=a<<c-32;return 0}function Un(a,b,c){a=a|0;b=b|0;c=c|0;if((c|0)<32){F=b>>>c;return a>>>c|(b&(1<<c)-1)<<32-c}F=0;return b>>>c-32|0}function Vn(a,b,c){a=a|0;b=b|0;c=c|0;if((c|0)<32){F=b>>c;return a>>>c|(b&(1<<c)-1)<<32-c}F=(b|0)<0?-1:0;return b>>c-32|0}function Wn(b){b=b|0;var c=0;c=a[n+(b>>>24)|0]|0;if((c|0)<8)return c|0;c=a[n+(b>>16&255)|0]|0;if((c|0)<8)return c+8|0;c=a[n+(b>>8&255)|0]|0;if((c|0)<8)return c+16|0;return(a[n+(b&255)|0]|0)+24|0}function Xn(b){b=b|0;var c=0;c=a[m+(b&255)|0]|0;if((c|0)<8)return c|0;c=a[m+(b>>8&255)|0]|0;if((c|0)<8)return c+8|0;c=a[m+(b>>16&255)|0]|0;if((c|0)<8)return c+16|0;return(a[m+(b>>>24)|0]|0)+24|0}function Yn(a,b){a=a|0;b=b|0;var c=0,d=0,e=0,f=0;c=a&65535;d=b&65535;e=ba(d,c)|0;f=a>>>16;a=(e>>>16)+(ba(d,f)|0)|0;d=b>>>16;b=ba(d,c)|0;return(F=(a>>>16)+(ba(d,f)|0)+(((a&65535)+b|0)>>>16)|0,a+b<<16|e&65535|0)|0}function Zn(a,b,c,d){a=a|0;b=b|0;c=c|0;d=d|0;var e=0,f=0,g=0,h=0,i=0;e=b>>31|((b|0)<0?-1:0)<<1;f=((b|0)<0?-1:0)>>31|((b|0)<0?-1:0)<<1;g=d>>31|((d|0)<0?-1:0)<<1;h=((d|0)<0?-1:0)>>31|((d|0)<0?-1:0)<<1;i=Sn(e^a,f^b,e,f)|0;b=F;a=g^e;e=h^f;f=Sn((co(i,b,Sn(g^c,h^d,g,h)|0,F,0)|0)^a,F^e,a,e)|0;return(F=F,f)|0}function _n(a,b,d,e){a=a|0;b=b|0;d=d|0;e=e|0;var f=0,g=0,h=0,j=0,k=0,l=0,m=0;f=i;i=i+8|0;g=f|0;h=b>>31|((b|0)<0?-1:0)<<1;j=((b|0)<0?-1:0)>>31|((b|0)<0?-1:0)<<1;k=e>>31|((e|0)<0?-1:0)<<1;l=((e|0)<0?-1:0)>>31|((e|0)<0?-1:0)<<1;m=Sn(h^a,j^b,h,j)|0;b=F;co(m,b,Sn(k^d,l^e,k,l)|0,F,g)|0;l=Sn(c[g>>2]^h,c[g+4>>2]^j,h,j)|0;j=F;i=f;return(F=j,l)|0}function $n(a,b,c,d){a=a|0;b=b|0;c=c|0;d=d|0;var e=0,f=0;e=a;a=c;c=Yn(e,a)|0;f=F;return(F=(ba(b,a)|0)+(ba(d,e)|0)+f|f&0,c|0|0)|0}function ao(a,b,c,d){a=a|0;b=b|0;c=c|0;d=d|0;var e=0;e=co(a,b,c,d,0)|0;return(F=F,e)|0}function bo(a,b,d,e){a=a|0;b=b|0;d=d|0;e=e|0;var f=0,g=0;f=i;i=i+8|0;g=f|0;co(a,b,d,e,g)|0;i=f;return(F=c[g+4>>2]|0,c[g>>2]|0)|0}function co(a,b,d,e,f){a=a|0;b=b|0;d=d|0;e=e|0;f=f|0;var g=0,h=0,i=0,j=0,k=0,l=0,m=0,n=0,o=0,p=0,q=0,r=0,s=0,t=0,u=0,v=0,w=0,x=0,y=0,z=0,A=0,B=0,C=0,D=0,E=0,G=0,H=0,I=0,J=0,K=0,L=0,M=0;g=a;h=b;i=h;j=d;k=e;l=k;if((i|0)==0){m=(f|0)!=0;if((l|0)==0){if(m){c[f>>2]=(g>>>0)%(j>>>0);c[f+4>>2]=0}n=0;o=(g>>>0)/(j>>>0)>>>0;return(F=n,o)|0}else{if(!m){n=0;o=0;return(F=n,o)|0}c[f>>2]=a|0;c[f+4>>2]=b&0;n=0;o=0;return(F=n,o)|0}}m=(l|0)==0;do{if((j|0)==0){if(m){if((f|0)!=0){c[f>>2]=(i>>>0)%(j>>>0);c[f+4>>2]=0}n=0;o=(i>>>0)/(j>>>0)>>>0;return(F=n,o)|0}if((g|0)==0){if((f|0)!=0){c[f>>2]=0;c[f+4>>2]=(i>>>0)%(l>>>0)}n=0;o=(i>>>0)/(l>>>0)>>>0;return(F=n,o)|0}p=l-1|0;if((p&l|0)==0){if((f|0)!=0){c[f>>2]=a|0;c[f+4>>2]=p&i|b&0}n=0;o=i>>>((Xn(l|0)|0)>>>0);return(F=n,o)|0}p=(Wn(l|0)|0)-(Wn(i|0)|0)|0;if(p>>>0<=30){q=p+1|0;r=31-p|0;s=q;t=i<<r|g>>>(q>>>0);u=i>>>(q>>>0);v=0;w=g<<r;break}if((f|0)==0){n=0;o=0;return(F=n,o)|0}c[f>>2]=a|0;c[f+4>>2]=h|b&0;n=0;o=0;return(F=n,o)|0}else{if(!m){r=(Wn(l|0)|0)-(Wn(i|0)|0)|0;if(r>>>0<=31){q=r+1|0;p=31-r|0;x=r-31>>31;s=q;t=g>>>(q>>>0)&x|i<<p;u=i>>>(q>>>0)&x;v=0;w=g<<p;break}if((f|0)==0){n=0;o=0;return(F=n,o)|0}c[f>>2]=a|0;c[f+4>>2]=h|b&0;n=0;o=0;return(F=n,o)|0}p=j-1|0;if((p&j|0)!=0){x=(Wn(j|0)|0)+33-(Wn(i|0)|0)|0;q=64-x|0;r=32-x|0;y=r>>31;z=x-32|0;A=z>>31;s=x;t=r-1>>31&i>>>(z>>>0)|(i<<r|g>>>(x>>>0))&A;u=A&i>>>(x>>>0);v=g<<q&y;w=(i<<q|g>>>(z>>>0))&y|g<<r&x-33>>31;break}if((f|0)!=0){c[f>>2]=p&g;c[f+4>>2]=0}if((j|0)==1){n=h|b&0;o=a|0|0;return(F=n,o)|0}else{p=Xn(j|0)|0;n=i>>>(p>>>0)|0;o=i<<32-p|g>>>(p>>>0)|0;return(F=n,o)|0}}}while(0);if((s|0)==0){B=w;C=v;D=u;E=t;G=0;H=0}else{g=d|0|0;d=k|e&0;e=Rn(g,d,-1,-1)|0;k=F;i=w;w=v;v=u;u=t;t=s;s=0;while(1){I=w>>>31|i<<1;J=s|w<<1;j=u<<1|i>>>31|0;a=u>>>31|v<<1|0;Sn(e,k,j,a)|0;b=F;h=b>>31|((b|0)<0?-1:0)<<1;K=h&1;L=Sn(j,a,h&g,(((b|0)<0?-1:0)>>31|((b|0)<0?-1:0)<<1)&d)|0;M=F;b=t-1|0;if((b|0)==0){break}else{i=I;w=J;v=M;u=L;t=b;s=K}}B=I;C=J;D=M;E=L;G=0;H=K}K=C;C=0;if((f|0)!=0){c[f>>2]=E;c[f+4>>2]=D}n=(K|0)>>>31|(B|C)<<1|(C<<1|K>>>31)&0|G;o=(K<<1|0>>>31)&-2|H;return(F=n,o)|0}function eo(){Pa()}function fo(a,b,c,d,e,f){a=a|0;b=b|0;c=c|0;d=d|0;e=e|0;f=f|0;hb[a&7](b|0,c|0,d|0,e|0,f|0)}function go(a,b,c,d,e,f,g){a=a|0;b=b|0;c=c|0;d=d|0;e=e|0;f=f|0;g=+g;return+ib[a&3](b|0,c|0,d|0,e|0,f|0,+g)}function ho(a,b){a=a|0;b=b|0;jb[a&255](b|0)}function io(a,b,c){a=a|0;b=b|0;c=c|0;kb[a&255](b|0,c|0)}function jo(a,b,c,d){a=a|0;b=b|0;c=c|0;d=d|0;return lb[a&31](b|0,c|0,d|0)|0}function ko(a,b){a=a|0;b=b|0;return mb[a&15](b|0)|0}function lo(a,b,c){a=a|0;b=b|0;c=+c;return+nb[a&31](b|0,+c)}function mo(a,b,c,d){a=a|0;b=b|0;c=c|0;d=d|0;ob[a&31](b|0,c|0,d|0)}function no(a){a=a|0;pb[a&3]()}function oo(a,b,c,d,e){a=a|0;b=b|0;c=c|0;d=d|0;e=e|0;return qb[a&15](b|0,c|0,d|0,e|0)|0}function po(a,b,c,d){a=a|0;b=b|0;c=c|0;d=+d;rb[a&63](b|0,c|0,+d)}function qo(a,b,c,d,e,f,g){a=a|0;b=b|0;c=c|0;d=d|0;e=e|0;f=f|0;g=g|0;sb[a&31](b|0,c|0,d|0,e|0,f|0,g|0)}function ro(a,b,c){a=a|0;b=b|0;c=c|0;return tb[a&63](b|0,c|0)|0}function so(a,b,c,d,e,f){a=a|0;b=b|0;c=c|0;d=d|0;e=e|0;f=f|0;return ub[a&31](b|0,c|0,d|0,e|0,f|0)|0}function to(a,b,c,d,e,f){a=a|0;b=b|0;c=c|0;d=d|0;e=e|0;f=+f;return+vb[a&3](b|0,c|0,d|0,e|0,+f)}function uo(a,b,c,d,e){a=a|0;b=b|0;c=c|0;d=d|0;e=e|0;wb[a&63](b|0,c|0,d|0,e|0)}function vo(a,b,c,d,e){a=a|0;b=b|0;c=c|0;d=d|0;e=e|0;ca(0)}function wo(a,b,c,d,e,f){a=a|0;b=b|0;c=c|0;d=d|0;e=e|0;f=+f;ca(1);return 0.0}function xo(a){a=a|0;ca(2)}function yo(a,b){a=a|0;b=b|0;ca(3)}function zo(a,b,c){a=a|0;b=b|0;c=c|0;ca(4);return 0}function Ao(a){a=a|0;ca(5);return 0}function Bo(a,b){a=a|0;b=+b;ca(6);return 0.0}function Co(a,b,c){a=a|0;b=b|0;c=c|0;ca(7)}function Do(){ca(8)}function Eo(a,b,c,d){a=a|0;b=b|0;c=c|0;d=d|0;ca(9);return 0}function Fo(a,b,c){a=a|0;b=b|0;c=+c;ca(10)}function Go(a,b,c,d,e,f){a=a|0;b=b|0;c=c|0;d=d|0;e=e|0;f=f|0;ca(11)}function Ho(a,b){a=a|0;b=b|0;ca(12);return 0}function Io(a,b,c,d,e){a=a|0;b=b|0;c=c|0;d=d|0;e=e|0;ca(13);return 0}function Jo(a,b,c,d,e){a=a|0;b=b|0;c=c|0;d=d|0;e=+e;ca(14);return 0.0}function Ko(a,b,c,d){a=a|0;b=b|0;c=c|0;d=d|0;ca(15)}




// EMSCRIPTEN_END_FUNCS
var hb=[vo,vo,Bn,vo,Cn,vo,vo,vo];var ib=[wo,wo,Vd,wo];var jb=[xo,xo,lm,xo,pi,xo,sm,xo,Ti,xo,rm,xo,kj,xo,uh,xo,Ec,xo,Bk,xo,Xd,xo,Yi,xo,rk,xo,bh,xo,Nh,xo,Tg,xo,Hg,xo,Si,xo,Gh,xo,Pm,xo,sn,xo,je,xo,vg,xo,kf,xo,Ni,xo,Hj,xo,wh,xo,Um,xo,Uf,xo,vn,xo,Cc,xo,un,xo,qi,xo,uj,xo,jj,xo,bi,xo,Di,xo,oj,xo,Gg,xo,Xb,xo,ek,xo,Fm,xo,Cm,xo,yj,xo,ci,xo,Ln,xo,Xj,xo,Hh,xo,Rg,xo,Dc,xo,fm,xo,Oi,xo,$m,xo,tg,xo,Lh,xo,tj,xo,af,xo,jg,xo,Hf,xo,Hi,xo,km,xo,vm,xo,$e,xo,zj,xo,Ci,xo,Ai,xo,pj,xo,um,xo,Fg,xo,Xi,xo,ie,xo,vh,xo,Cf,xo,Hk,xo,dh,xo,Re,xo,_i,xo,zm,xo,Om,xo,fk,xo,mn,xo,Vf,xo,ke,xo,Ii,xo,Aj,xo,Gm,xo,Tm,xo,Kn,xo,Gf,xo,Qe,xo,$d,xo,ug,xo,ch,xo,ig,xo,oi,xo,xk,xo,Yk,xo,Sg,xo,gm,xo,tn,xo,Bm,xo,di,xo,Fh,xo,Wf,xo,Vb,xo,jf,xo,Mh,xo,gg,xo,wn,xo,Td,xo,Rb,xo,nn,xo,xo,xo,xo,xo,xo,xo,xo,xo,xo,xo,xo,xo,xo,xo,xo,xo,xo,xo,xo,xo,xo,xo,xo,xo,xo,xo,xo,xo,xo,xo,xo,xo];var kb=[yo,yo,Pf,yo,Zf,yo,Kg,yo,Tf,yo,zg,yo,gh,yo,Th,yo,qg,yo,Nf,yo,hh,yo,xi,yo,Uh,yo,Zg,yo,sk,yo,Qi,yo,ng,yo,Vi,yo,zh,yo,be,yo,Bi,yo,Wg,yo,tk,yo,dn,yo,lg,yo,fi,yo,wj,yo,hi,yo,Mf,yo,ag,yo,xh,yo,ti,yo,ae,yo,jh,yo,Yg,yo,mg,yo,Fi,yo,Ch,yo,Ph,yo,fh,yo,gi,yo,Bh,yo,ui,yo,wi,yo,bg,yo,hg,yo,Yf,yo,Kf,yo,hj,yo,Cg,yo,Og,yo,si,yo,mj,yo,Bg,yo,kh,yo,Gj,yo,Ng,yo,xg,yo,Zi,yo,Lg,yo,Ug,yo,rj,yo,Rh,yo,Qf,yo,Qh,yo,pg,yo,yg,yo,ki,yo,ji,yo,yh,yo,Vg,yo,_f,yo,Jg,yo,yo,yo,yo,yo,yo,yo,yo,yo,yo,yo,yo,yo,yo,yo,yo,yo,yo,yo,yo,yo,yo,yo,yo,yo,yo,yo,yo,yo,yo,yo,yo,yo,yo,yo,yo,yo,yo,yo,yo,yo,yo,yo,yo,yo,yo,yo,yo,yo,yo,yo,yo,yo,yo,yo,yo,yo,yo,yo,yo,yo,yo,yo,yo,yo,yo,yo,yo,yo,yo,yo,yo,yo,yo,yo,yo,yo,yo,yo,yo,yo,yo,yo,yo,yo,yo,yo,yo,yo,yo,yo,yo,yo,yo,yo,yo,yo,yo,yo,yo,yo,yo,yo,yo,yo,yo,yo,yo,yo,yo,yo];var lb=[zo,zo,xm,zo,xn,zo,Rm,zo,Zd,zo,Ee,zo,we,zo,Dm,zo,Ff,zo,Jm,zo,Xe,zo,Le,zo,zo,zo,zo,zo,zo,zo,zo,zo];var mb=[Ao,Ao,Mn,Ao,Ce,Ao,Ke,Ao,Im,Ao,Ue,Ao,ve,Ao,Ao,Ao];var nb=[Bo,Bo,Wh,Bo,mi,Bo,Sf,Bo,dg,Bo,Eg,Bo,sg,Bo,$g,Bo,mh,Bo,Qg,Bo,zi,Bo,Eh,Bo,Bo,Bo,Bo,Bo,Bo,Bo,Bo,Bo];var ob=[Co,Co,ee,Co,ge,Co,he,Co,vk,Co,Gk,Co,ce,Co,Wj,Co,uk,Co,wk,Co,Co,Co,Co,Co,Co,Co,Co,Co,Co,Co,Co,Co];var pb=[Do,Do,eo,Do];var qb=[Eo,Eo,Em,Eo,ym,Eo,Jf,Eo,If,Eo,Eo,Eo,Eo,Eo,Eo,Eo];var rb=[Fo,Fo,_e,Fo,Vh,Fo,Ie,Fo,Rf,Fo,li,Fo,cg,Fo,Pg,Fo,rg,Fo,Nm,Fo,Pe,Fo,yi,Fo,lh,Fo,_g,Fo,Dg,Fo,Dh,Fo,Ae,Fo,Fo,Fo,Fo,Fo,Fo,Fo,Fo,Fo,Fo,Fo,Fo,Fo,Fo,Fo,Fo,Fo,Fo,Fo,Fo,Fo,Fo,Fo,Fo,Fo,Fo,Fo,Fo,Fo,Fo,Fo];var sb=[Go,Go,En,Go,xe,Go,Km,Go,Dn,Go,Me,Go,Fe,Go,Bf,Go,We,Go,Go,Go,Go,Go,Go,Go,Go,Go,Go,Go,Go,Go,Go,Go];var tb=[Ho,Ho,Ag,Ho,hm,Ho,vi,Ho,Zk,Ho,cn,Ho,wm,Ho,Be,Ho,Qm,Ho,Wd,Ho,Hm,Ho,Je,Ho,_d,Ho,Yd,Ho,gl,Ho,Sh,Ho,mk,Ho,Of,Ho,im,Ho,_k,Ho,nm,Ho,ih,Ho,Ah,Ho,og,Ho,il,Ho,Am,Ho,ii,Ho,ue,Ho,$f,Ho,Te,Ho,Mg,Ho,Xg,Ho];var ub=[Io,Io,vj,Io,Ei,Io,Ge,Io,gj,Io,Ui,Io,Lm,Io,Pi,Io,Ye,Io,lj,Io,ye,Io,Ne,Io,qj,Io,Io,Io,Io,Io,Io,Io];var vb=[Jo,Jo,Ud,Jo];var wb=[Ko,Ko,He,Ko,Gi,Ko,Mm,Ko,Wi,Ko,yn,Ko,xj,Ko,jm,Ko,Ri,Ko,zn,Ko,de,Ko,Ze,Ko,ze,Ko,sj,Ko,Oe,Ko,mm,Ko,tm,Ko,ij,Ko,fe,Ko,nj,Ko,Ko,Ko,Ko,Ko,Ko,Ko,Ko,Ko,Ko,Ko,Ko,Ko,Ko,Ko,Ko,Ko,Ko,Ko,Ko,Ko,Ko,Ko,Ko,Ko];return{_b2PrismaticJoint_IsMotorEnabled:ld,_b2Body_GetWorldVector:uc,_strlen:Qn,_b2Body_GetLocalVector:nc,_b2PrismaticJoint_GetMotorSpeed:id,_b2RopeJointDef_Create:zd,_b2World_CreateParticleSystem:Hc,_b2ParticleSystem_SetRadius:Sd,_b2FrictionJointDef_Create:Vc,_b2World_RayCast:Nc,_b2MotorJointDef_InitializeAndCreate:cd,_b2GearJoint_GetRatio:Xc,_b2Body_GetLocalPoint:mc,_memcpy:On,_b2RevoluteJoint_EnableLimit:rd,_b2Body_DestroyFixture:hc,_b2Body_GetWorldCenter:sc,_b2PolygonShape_CreateFixture_3:Wb,_b2PolygonShape_CreateFixture_6:_b,_b2PolygonShape_CreateFixture_7:$b,_b2PolygonShape_CreateFixture_4:Yb,_b2PolygonShape_CreateFixture_5:Zb,_b2Body_SetAwake:vc,_b2ParticleGroup_DestroyParticles:Id,_b2PrismaticJoint_EnableLimit:fd,_b2Fixture_TestPoint:Bc,_b2MotorJoint_SetLinearOffset:ad,_free:Gn,_b2ParticleSystem_CreateParticle:Ld,_b2Body_SetLinearVelocity:xc,_b2Body_SetAngularVelocity:wc,_b2CircleShape_CreateParticleGroup:Sb,_b2WeldJointDef_Create:Ad,_b2Body_GetAngularVelocity:jc,_b2Body_ApplyForceToCenter:fc,_b2DistanceJointDef_InitializeAndCreate:Uc,_b2Body_ApplyForce:ec,_b2RevoluteJoint_SetMotorSpeed:yd,_b2Body_GetPosition:pc,_b2World_CreateBody:Gc,_b2Body_GetLinearVelocity:lc,_b2World_Create:Fc,_b2Body_SetMassData:yc,_b2Body_GetAngle:ic,_b2ParticleSystem_GetColorBuffer:Md,_b2DistanceJointDef_Create:Tc,_b2ParticleSystem_GetPositionBuffer:Od,_b2CircleShape_DestroyParticlesInShape:Tb,_b2World_QueryAABB:Mc,_b2PolygonShape_DestroyParticlesInShape_4:cc,_b2RevoluteJointDef_InitializeAndCreate:xd,_b2GearJointDef_Create:Yc,__GLOBAL__I_a:le,_b2PrismaticJoint_IsLimitEnabled:kd,_b2Contact_GetWorldManifold:Sc,_b2WheelJoint_SetSpringFrequencyHz:Dd,_b2Body_ApplyTorque:gc,_b2ParticleGroup_ApplyLinearImpulse:Hd,_b2PrismaticJointDef_InitializeAndCreate:od,_memset:Nn,_b2MotorJointDef_Create:bd,_b2World_DestroyBody:Jc,_b2RevoluteJointDef_Create:wd,_b2ParticleGroup_GetParticleCount:Kd,_b2World_SetGravity:Pc,_b2PrismaticJoint_EnableMotor:gd,_b2PolygonShape_CreateParticleGroup_4:bc,_b2PolygonShape_CreateFixture_8:ac,_b2ParticleSystem_SetDamping:Qd,_b2Body_GetWorldPoint:tc,_b2WheelJoint_SetMotorSpeed:Cd,_b2EdgeShape_CreateFixture:Ub,_b2World_SetContactListener:Oc,_b2Body_SetType:Ac,_b2Body_SetTransform:zc,_b2CircleShape_CreateFixture:Qb,_b2RevoluteJoint_GetJointAngle:td,_b2ParticleGroup_ApplyForce:Gd,_malloc:Fn,_b2Contact_GetManifold:Rc,_b2RevoluteJoint_IsMotorEnabled:vd,_b2ParticleSystem_GetVelocityBuffer:Pd,_b2RevoluteJoint_IsLimitEnabled:ud,_b2World_DestroyParticleSystem:Lc,_b2RevoluteJoint_EnableMotor:sd,_b2Body_GetTransform:qc,_b2WeldJointDef_InitializeAndCreate:Bd,_b2FrictionJointDef_InitializeAndCreate:Wc,_b2Body_GetType:rc,_b2Manifold_GetPointCount:Ob,_b2ParticleSystem_SetDensity:Rd,_b2PrismaticJoint_GetMotorForce:jd,_b2Joint_GetBodyA:Zc,_b2PulleyJointDef_InitializeAndCreate:qd,_b2Joint_GetBodyB:_c,_b2ParticleSystem_GetParticleCount:Nd,_b2MouseJoint_SetTarget:dd,_b2Body_ApplyAngularImpulse:dc,_b2ChainShape_CreateFixture:Pb,_b2World_DestroyJoint:Kc,_b2MotorJoint_SetAngularOffset:$c,_b2World_Delete:Ic,_b2PrismaticJoint_GetJointTranslation:hd,_b2Body_GetMass:oc,_b2MouseJointDef_Create:ed,_b2ParticleGroup_GetBufferIndex:Jd,_b2WheelJointDef_InitializeAndCreate:Fd,_b2World_Step:Qc,_b2PrismaticJointDef_Create:nd,_b2WheelJointDef_Create:Ed,_b2PulleyJointDef_Create:pd,_b2Body_GetInertia:kc,_memmove:Pn,_b2PrismaticJoint_SetMotorSpeed:md,runPostSets:Nb,stackAlloc:xb,stackSave:yb,stackRestore:zb,setThrew:Ab,setTempRet0:Db,setTempRet1:Eb,setTempRet2:Fb,setTempRet3:Gb,setTempRet4:Hb,setTempRet5:Ib,setTempRet6:Jb,setTempRet7:Kb,setTempRet8:Lb,setTempRet9:Mb,dynCall_viiiii:fo,dynCall_fiiiiif:go,dynCall_vi:ho,dynCall_vii:io,dynCall_iiii:jo,dynCall_ii:ko,dynCall_fif:lo,dynCall_viii:mo,dynCall_v:no,dynCall_iiiii:oo,dynCall_viif:po,dynCall_viiiiii:qo,dynCall_iii:ro,dynCall_iiiiii:so,dynCall_fiiiif:to,dynCall_viiii:uo}})


// EMSCRIPTEN_END_ASM
({ "Math": Math, "Int8Array": Int8Array, "Int16Array": Int16Array, "Int32Array": Int32Array, "Uint8Array": Uint8Array, "Uint16Array": Uint16Array, "Uint32Array": Uint32Array, "Float32Array": Float32Array, "Float64Array": Float64Array }, { "abort": abort, "assert": assert, "asmPrintInt": asmPrintInt, "asmPrintFloat": asmPrintFloat, "min": Math_min, "invoke_viiiii": invoke_viiiii, "invoke_fiiiiif": invoke_fiiiiif, "invoke_vi": invoke_vi, "invoke_vii": invoke_vii, "invoke_iiii": invoke_iiii, "invoke_ii": invoke_ii, "invoke_fif": invoke_fif, "invoke_viii": invoke_viii, "invoke_v": invoke_v, "invoke_iiiii": invoke_iiiii, "invoke_viif": invoke_viif, "invoke_viiiiii": invoke_viiiiii, "invoke_iii": invoke_iii, "invoke_iiiiii": invoke_iiiiii, "invoke_fiiiif": invoke_fiiiif, "invoke_viiii": invoke_viiii, "_llvm_lifetime_end": _llvm_lifetime_end, "_cosf": _cosf, "_b2WorldRayCastCallback": _b2WorldRayCastCallback, "___cxa_call_unexpected": ___cxa_call_unexpected, "_floorf": _floorf, "___cxa_throw": ___cxa_throw, "_sinf": _sinf, "_abort": _abort, "___cxa_end_catch": ___cxa_end_catch, "_b2WorldBeginContactBody": _b2WorldBeginContactBody, "___cxa_free_exception": ___cxa_free_exception, "_fflush": _fflush, "_sqrtf": _sqrtf, "_sysconf": _sysconf, "___setErrNo": ___setErrNo, "_exit": _exit, "___cxa_find_matching_catch": ___cxa_find_matching_catch, "___cxa_allocate_exception": ___cxa_allocate_exception, "___cxa_pure_virtual": ___cxa_pure_virtual, "_b2WorldEndContactBody": _b2WorldEndContactBody, "_time": _time, "___cxa_is_number_type": ___cxa_is_number_type, "___cxa_does_inherit": ___cxa_does_inherit, "__ZSt9terminatev": __ZSt9terminatev, "_b2WorldPreSolve": _b2WorldPreSolve, "___cxa_begin_catch": ___cxa_begin_catch, "_emscripten_memcpy_big": _emscripten_memcpy_big, "__ZSt18uncaught_exceptionv": __ZSt18uncaught_exceptionv, "_b2WorldQueryAABB": _b2WorldQueryAABB, "_sbrk": _sbrk, "__ZNSt9exceptionD2Ev": __ZNSt9exceptionD2Ev, "___errno_location": ___errno_location, "___gxx_personality_v0": ___gxx_personality_v0, "_b2WorldPostSolve": _b2WorldPostSolve, "_llvm_lifetime_start": _llvm_lifetime_start, "___resumeException": ___resumeException, "__exit": __exit, "STACKTOP": STACKTOP, "STACK_MAX": STACK_MAX, "tempDoublePtr": tempDoublePtr, "ABORT": ABORT, "cttz_i8": cttz_i8, "ctlz_i8": ctlz_i8, "NaN": NaN, "Infinity": Infinity, "__ZTVN10__cxxabiv120__si_class_type_infoE": __ZTVN10__cxxabiv120__si_class_type_infoE, "__ZTVN10__cxxabiv117__class_type_infoE": __ZTVN10__cxxabiv117__class_type_infoE }, buffer);
var _b2PrismaticJoint_IsMotorEnabled = Module["_b2PrismaticJoint_IsMotorEnabled"] = asm["_b2PrismaticJoint_IsMotorEnabled"];
var _b2Body_GetWorldVector = Module["_b2Body_GetWorldVector"] = asm["_b2Body_GetWorldVector"];
var _strlen = Module["_strlen"] = asm["_strlen"];
var _b2Body_GetLocalVector = Module["_b2Body_GetLocalVector"] = asm["_b2Body_GetLocalVector"];
var _b2PrismaticJoint_GetMotorSpeed = Module["_b2PrismaticJoint_GetMotorSpeed"] = asm["_b2PrismaticJoint_GetMotorSpeed"];
var _b2RopeJointDef_Create = Module["_b2RopeJointDef_Create"] = asm["_b2RopeJointDef_Create"];
var _b2World_CreateParticleSystem = Module["_b2World_CreateParticleSystem"] = asm["_b2World_CreateParticleSystem"];
var _b2ParticleSystem_SetRadius = Module["_b2ParticleSystem_SetRadius"] = asm["_b2ParticleSystem_SetRadius"];
var _b2FrictionJointDef_Create = Module["_b2FrictionJointDef_Create"] = asm["_b2FrictionJointDef_Create"];
var _b2World_RayCast = Module["_b2World_RayCast"] = asm["_b2World_RayCast"];
var _b2MotorJointDef_InitializeAndCreate = Module["_b2MotorJointDef_InitializeAndCreate"] = asm["_b2MotorJointDef_InitializeAndCreate"];
var _b2GearJoint_GetRatio = Module["_b2GearJoint_GetRatio"] = asm["_b2GearJoint_GetRatio"];
var _b2Body_GetLocalPoint = Module["_b2Body_GetLocalPoint"] = asm["_b2Body_GetLocalPoint"];
var _memcpy = Module["_memcpy"] = asm["_memcpy"];
var _b2RevoluteJoint_EnableLimit = Module["_b2RevoluteJoint_EnableLimit"] = asm["_b2RevoluteJoint_EnableLimit"];
var _b2Body_DestroyFixture = Module["_b2Body_DestroyFixture"] = asm["_b2Body_DestroyFixture"];
var _b2Body_GetWorldCenter = Module["_b2Body_GetWorldCenter"] = asm["_b2Body_GetWorldCenter"];
var _b2PolygonShape_CreateFixture_3 = Module["_b2PolygonShape_CreateFixture_3"] = asm["_b2PolygonShape_CreateFixture_3"];
var _b2PolygonShape_CreateFixture_6 = Module["_b2PolygonShape_CreateFixture_6"] = asm["_b2PolygonShape_CreateFixture_6"];
var _b2PolygonShape_CreateFixture_7 = Module["_b2PolygonShape_CreateFixture_7"] = asm["_b2PolygonShape_CreateFixture_7"];
var _b2PolygonShape_CreateFixture_4 = Module["_b2PolygonShape_CreateFixture_4"] = asm["_b2PolygonShape_CreateFixture_4"];
var _b2PolygonShape_CreateFixture_5 = Module["_b2PolygonShape_CreateFixture_5"] = asm["_b2PolygonShape_CreateFixture_5"];
var _b2Body_SetAwake = Module["_b2Body_SetAwake"] = asm["_b2Body_SetAwake"];
var _b2ParticleGroup_DestroyParticles = Module["_b2ParticleGroup_DestroyParticles"] = asm["_b2ParticleGroup_DestroyParticles"];
var _b2PrismaticJoint_EnableLimit = Module["_b2PrismaticJoint_EnableLimit"] = asm["_b2PrismaticJoint_EnableLimit"];
var _b2Fixture_TestPoint = Module["_b2Fixture_TestPoint"] = asm["_b2Fixture_TestPoint"];
var _b2MotorJoint_SetLinearOffset = Module["_b2MotorJoint_SetLinearOffset"] = asm["_b2MotorJoint_SetLinearOffset"];
var _free = Module["_free"] = asm["_free"];
var _b2ParticleSystem_CreateParticle = Module["_b2ParticleSystem_CreateParticle"] = asm["_b2ParticleSystem_CreateParticle"];
var _b2Body_SetLinearVelocity = Module["_b2Body_SetLinearVelocity"] = asm["_b2Body_SetLinearVelocity"];
var _b2Body_SetAngularVelocity = Module["_b2Body_SetAngularVelocity"] = asm["_b2Body_SetAngularVelocity"];
var _b2CircleShape_CreateParticleGroup = Module["_b2CircleShape_CreateParticleGroup"] = asm["_b2CircleShape_CreateParticleGroup"];
var _b2WeldJointDef_Create = Module["_b2WeldJointDef_Create"] = asm["_b2WeldJointDef_Create"];
var _b2Body_GetAngularVelocity = Module["_b2Body_GetAngularVelocity"] = asm["_b2Body_GetAngularVelocity"];
var _b2Body_ApplyForceToCenter = Module["_b2Body_ApplyForceToCenter"] = asm["_b2Body_ApplyForceToCenter"];
var _b2DistanceJointDef_InitializeAndCreate = Module["_b2DistanceJointDef_InitializeAndCreate"] = asm["_b2DistanceJointDef_InitializeAndCreate"];
var _b2Body_ApplyForce = Module["_b2Body_ApplyForce"] = asm["_b2Body_ApplyForce"];
var _b2RevoluteJoint_SetMotorSpeed = Module["_b2RevoluteJoint_SetMotorSpeed"] = asm["_b2RevoluteJoint_SetMotorSpeed"];
var _b2Body_GetPosition = Module["_b2Body_GetPosition"] = asm["_b2Body_GetPosition"];
var _b2World_CreateBody = Module["_b2World_CreateBody"] = asm["_b2World_CreateBody"];
var _b2Body_GetLinearVelocity = Module["_b2Body_GetLinearVelocity"] = asm["_b2Body_GetLinearVelocity"];
var _b2World_Create = Module["_b2World_Create"] = asm["_b2World_Create"];
var _b2Body_SetMassData = Module["_b2Body_SetMassData"] = asm["_b2Body_SetMassData"];
var _b2Body_GetAngle = Module["_b2Body_GetAngle"] = asm["_b2Body_GetAngle"];
var _b2ParticleSystem_GetColorBuffer = Module["_b2ParticleSystem_GetColorBuffer"] = asm["_b2ParticleSystem_GetColorBuffer"];
var _b2DistanceJointDef_Create = Module["_b2DistanceJointDef_Create"] = asm["_b2DistanceJointDef_Create"];
var _b2ParticleSystem_GetPositionBuffer = Module["_b2ParticleSystem_GetPositionBuffer"] = asm["_b2ParticleSystem_GetPositionBuffer"];
var _b2CircleShape_DestroyParticlesInShape = Module["_b2CircleShape_DestroyParticlesInShape"] = asm["_b2CircleShape_DestroyParticlesInShape"];
var _b2World_QueryAABB = Module["_b2World_QueryAABB"] = asm["_b2World_QueryAABB"];
var _b2PolygonShape_DestroyParticlesInShape_4 = Module["_b2PolygonShape_DestroyParticlesInShape_4"] = asm["_b2PolygonShape_DestroyParticlesInShape_4"];
var _b2RevoluteJointDef_InitializeAndCreate = Module["_b2RevoluteJointDef_InitializeAndCreate"] = asm["_b2RevoluteJointDef_InitializeAndCreate"];
var _b2GearJointDef_Create = Module["_b2GearJointDef_Create"] = asm["_b2GearJointDef_Create"];
var __GLOBAL__I_a = Module["__GLOBAL__I_a"] = asm["__GLOBAL__I_a"];
var _b2PrismaticJoint_IsLimitEnabled = Module["_b2PrismaticJoint_IsLimitEnabled"] = asm["_b2PrismaticJoint_IsLimitEnabled"];
var _b2Contact_GetWorldManifold = Module["_b2Contact_GetWorldManifold"] = asm["_b2Contact_GetWorldManifold"];
var _b2WheelJoint_SetSpringFrequencyHz = Module["_b2WheelJoint_SetSpringFrequencyHz"] = asm["_b2WheelJoint_SetSpringFrequencyHz"];
var _b2Body_ApplyTorque = Module["_b2Body_ApplyTorque"] = asm["_b2Body_ApplyTorque"];
var _b2ParticleGroup_ApplyLinearImpulse = Module["_b2ParticleGroup_ApplyLinearImpulse"] = asm["_b2ParticleGroup_ApplyLinearImpulse"];
var _b2PrismaticJointDef_InitializeAndCreate = Module["_b2PrismaticJointDef_InitializeAndCreate"] = asm["_b2PrismaticJointDef_InitializeAndCreate"];
var _memset = Module["_memset"] = asm["_memset"];
var _b2MotorJointDef_Create = Module["_b2MotorJointDef_Create"] = asm["_b2MotorJointDef_Create"];
var _b2World_DestroyBody = Module["_b2World_DestroyBody"] = asm["_b2World_DestroyBody"];
var _b2RevoluteJointDef_Create = Module["_b2RevoluteJointDef_Create"] = asm["_b2RevoluteJointDef_Create"];
var _b2ParticleGroup_GetParticleCount = Module["_b2ParticleGroup_GetParticleCount"] = asm["_b2ParticleGroup_GetParticleCount"];
var _b2World_SetGravity = Module["_b2World_SetGravity"] = asm["_b2World_SetGravity"];
var _b2PrismaticJoint_EnableMotor = Module["_b2PrismaticJoint_EnableMotor"] = asm["_b2PrismaticJoint_EnableMotor"];
var _b2PolygonShape_CreateParticleGroup_4 = Module["_b2PolygonShape_CreateParticleGroup_4"] = asm["_b2PolygonShape_CreateParticleGroup_4"];
var _b2PolygonShape_CreateFixture_8 = Module["_b2PolygonShape_CreateFixture_8"] = asm["_b2PolygonShape_CreateFixture_8"];
var _b2ParticleSystem_SetDamping = Module["_b2ParticleSystem_SetDamping"] = asm["_b2ParticleSystem_SetDamping"];
var _b2Body_GetWorldPoint = Module["_b2Body_GetWorldPoint"] = asm["_b2Body_GetWorldPoint"];
var _b2WheelJoint_SetMotorSpeed = Module["_b2WheelJoint_SetMotorSpeed"] = asm["_b2WheelJoint_SetMotorSpeed"];
var _b2EdgeShape_CreateFixture = Module["_b2EdgeShape_CreateFixture"] = asm["_b2EdgeShape_CreateFixture"];
var _b2World_SetContactListener = Module["_b2World_SetContactListener"] = asm["_b2World_SetContactListener"];
var _b2Body_SetType = Module["_b2Body_SetType"] = asm["_b2Body_SetType"];
var _b2Body_SetTransform = Module["_b2Body_SetTransform"] = asm["_b2Body_SetTransform"];
var _b2CircleShape_CreateFixture = Module["_b2CircleShape_CreateFixture"] = asm["_b2CircleShape_CreateFixture"];
var _b2RevoluteJoint_GetJointAngle = Module["_b2RevoluteJoint_GetJointAngle"] = asm["_b2RevoluteJoint_GetJointAngle"];
var _b2ParticleGroup_ApplyForce = Module["_b2ParticleGroup_ApplyForce"] = asm["_b2ParticleGroup_ApplyForce"];
var _malloc = Module["_malloc"] = asm["_malloc"];
var _b2Contact_GetManifold = Module["_b2Contact_GetManifold"] = asm["_b2Contact_GetManifold"];
var _b2RevoluteJoint_IsMotorEnabled = Module["_b2RevoluteJoint_IsMotorEnabled"] = asm["_b2RevoluteJoint_IsMotorEnabled"];
var _b2ParticleSystem_GetVelocityBuffer = Module["_b2ParticleSystem_GetVelocityBuffer"] = asm["_b2ParticleSystem_GetVelocityBuffer"];
var _b2RevoluteJoint_IsLimitEnabled = Module["_b2RevoluteJoint_IsLimitEnabled"] = asm["_b2RevoluteJoint_IsLimitEnabled"];
var _b2World_DestroyParticleSystem = Module["_b2World_DestroyParticleSystem"] = asm["_b2World_DestroyParticleSystem"];
var _b2RevoluteJoint_EnableMotor = Module["_b2RevoluteJoint_EnableMotor"] = asm["_b2RevoluteJoint_EnableMotor"];
var _b2Body_GetTransform = Module["_b2Body_GetTransform"] = asm["_b2Body_GetTransform"];
var _b2WeldJointDef_InitializeAndCreate = Module["_b2WeldJointDef_InitializeAndCreate"] = asm["_b2WeldJointDef_InitializeAndCreate"];
var _b2FrictionJointDef_InitializeAndCreate = Module["_b2FrictionJointDef_InitializeAndCreate"] = asm["_b2FrictionJointDef_InitializeAndCreate"];
var _b2Body_GetType = Module["_b2Body_GetType"] = asm["_b2Body_GetType"];
var _b2Manifold_GetPointCount = Module["_b2Manifold_GetPointCount"] = asm["_b2Manifold_GetPointCount"];
var _b2ParticleSystem_SetDensity = Module["_b2ParticleSystem_SetDensity"] = asm["_b2ParticleSystem_SetDensity"];
var _b2PrismaticJoint_GetMotorForce = Module["_b2PrismaticJoint_GetMotorForce"] = asm["_b2PrismaticJoint_GetMotorForce"];
var _b2Joint_GetBodyA = Module["_b2Joint_GetBodyA"] = asm["_b2Joint_GetBodyA"];
var _b2PulleyJointDef_InitializeAndCreate = Module["_b2PulleyJointDef_InitializeAndCreate"] = asm["_b2PulleyJointDef_InitializeAndCreate"];
var _b2Joint_GetBodyB = Module["_b2Joint_GetBodyB"] = asm["_b2Joint_GetBodyB"];
var _b2ParticleSystem_GetParticleCount = Module["_b2ParticleSystem_GetParticleCount"] = asm["_b2ParticleSystem_GetParticleCount"];
var _b2MouseJoint_SetTarget = Module["_b2MouseJoint_SetTarget"] = asm["_b2MouseJoint_SetTarget"];
var _b2Body_ApplyAngularImpulse = Module["_b2Body_ApplyAngularImpulse"] = asm["_b2Body_ApplyAngularImpulse"];
var _b2ChainShape_CreateFixture = Module["_b2ChainShape_CreateFixture"] = asm["_b2ChainShape_CreateFixture"];
var _b2World_DestroyJoint = Module["_b2World_DestroyJoint"] = asm["_b2World_DestroyJoint"];
var _b2MotorJoint_SetAngularOffset = Module["_b2MotorJoint_SetAngularOffset"] = asm["_b2MotorJoint_SetAngularOffset"];
var _b2World_Delete = Module["_b2World_Delete"] = asm["_b2World_Delete"];
var _b2PrismaticJoint_GetJointTranslation = Module["_b2PrismaticJoint_GetJointTranslation"] = asm["_b2PrismaticJoint_GetJointTranslation"];
var _b2Body_GetMass = Module["_b2Body_GetMass"] = asm["_b2Body_GetMass"];
var _b2MouseJointDef_Create = Module["_b2MouseJointDef_Create"] = asm["_b2MouseJointDef_Create"];
var _b2ParticleGroup_GetBufferIndex = Module["_b2ParticleGroup_GetBufferIndex"] = asm["_b2ParticleGroup_GetBufferIndex"];
var _b2WheelJointDef_InitializeAndCreate = Module["_b2WheelJointDef_InitializeAndCreate"] = asm["_b2WheelJointDef_InitializeAndCreate"];
var _b2World_Step = Module["_b2World_Step"] = asm["_b2World_Step"];
var _b2PrismaticJointDef_Create = Module["_b2PrismaticJointDef_Create"] = asm["_b2PrismaticJointDef_Create"];
var _b2WheelJointDef_Create = Module["_b2WheelJointDef_Create"] = asm["_b2WheelJointDef_Create"];
var _b2PulleyJointDef_Create = Module["_b2PulleyJointDef_Create"] = asm["_b2PulleyJointDef_Create"];
var _b2Body_GetInertia = Module["_b2Body_GetInertia"] = asm["_b2Body_GetInertia"];
var _memmove = Module["_memmove"] = asm["_memmove"];
var _b2PrismaticJoint_SetMotorSpeed = Module["_b2PrismaticJoint_SetMotorSpeed"] = asm["_b2PrismaticJoint_SetMotorSpeed"];
var runPostSets = Module["runPostSets"] = asm["runPostSets"];
var dynCall_viiiii = Module["dynCall_viiiii"] = asm["dynCall_viiiii"];
var dynCall_fiiiiif = Module["dynCall_fiiiiif"] = asm["dynCall_fiiiiif"];
var dynCall_vi = Module["dynCall_vi"] = asm["dynCall_vi"];
var dynCall_vii = Module["dynCall_vii"] = asm["dynCall_vii"];
var dynCall_iiii = Module["dynCall_iiii"] = asm["dynCall_iiii"];
var dynCall_ii = Module["dynCall_ii"] = asm["dynCall_ii"];
var dynCall_fif = Module["dynCall_fif"] = asm["dynCall_fif"];
var dynCall_viii = Module["dynCall_viii"] = asm["dynCall_viii"];
var dynCall_v = Module["dynCall_v"] = asm["dynCall_v"];
var dynCall_iiiii = Module["dynCall_iiiii"] = asm["dynCall_iiiii"];
var dynCall_viif = Module["dynCall_viif"] = asm["dynCall_viif"];
var dynCall_viiiiii = Module["dynCall_viiiiii"] = asm["dynCall_viiiiii"];
var dynCall_iii = Module["dynCall_iii"] = asm["dynCall_iii"];
var dynCall_iiiiii = Module["dynCall_iiiiii"] = asm["dynCall_iiiiii"];
var dynCall_fiiiif = Module["dynCall_fiiiif"] = asm["dynCall_fiiiif"];
var dynCall_viiii = Module["dynCall_viiii"] = asm["dynCall_viiii"];

Runtime.stackAlloc = function(size) { return asm['stackAlloc'](size) };
Runtime.stackSave = function() { return asm['stackSave']() };
Runtime.stackRestore = function(top) { asm['stackRestore'](top) };

// TODO: strip out parts of this we do not need

//======= begin closure i64 code =======

// Copyright 2009 The Closure Library Authors. All Rights Reserved.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS-IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

/**
 * @fileoverview Defines a Long class for representing a 64-bit two's-complement
 * integer value, which faithfully simulates the behavior of a Java "long". This
 * implementation is derived from LongLib in GWT.
 *
 */

var i64Math = (function() { // Emscripten wrapper
  var goog = { math: {} };


  /**
   * Constructs a 64-bit two's-complement integer, given its low and high 32-bit
   * values as *signed* integers.  See the from* functions below for more
   * convenient ways of constructing Longs.
   *
   * The internal representation of a long is the two given signed, 32-bit values.
   * We use 32-bit pieces because these are the size of integers on which
   * Javascript performs bit-operations.  For operations like addition and
   * multiplication, we split each number into 16-bit pieces, which can easily be
   * multiplied within Javascript's floating-point representation without overflow
   * or change in sign.
   *
   * In the algorithms below, we frequently reduce the negative case to the
   * positive case by negating the input(s) and then post-processing the result.
   * Note that we must ALWAYS check specially whether those values are MIN_VALUE
   * (-2^63) because -MIN_VALUE == MIN_VALUE (since 2^63 cannot be represented as
   * a positive number, it overflows back into a negative).  Not handling this
   * case would often result in infinite recursion.
   *
   * @param {number} low  The low (signed) 32 bits of the long.
   * @param {number} high  The high (signed) 32 bits of the long.
   * @constructor
   */
  goog.math.Long = function(low, high) {
    /**
     * @type {number}
     * @private
     */
    this.low_ = low | 0;  // force into 32 signed bits.

    /**
     * @type {number}
     * @private
     */
    this.high_ = high | 0;  // force into 32 signed bits.
  };


  // NOTE: Common constant values ZERO, ONE, NEG_ONE, etc. are defined below the
  // from* methods on which they depend.


  /**
   * A cache of the Long representations of small integer values.
   * @type {!Object}
   * @private
   */
  goog.math.Long.IntCache_ = {};


  /**
   * Returns a Long representing the given (32-bit) integer value.
   * @param {number} value The 32-bit integer in question.
   * @return {!goog.math.Long} The corresponding Long value.
   */
  goog.math.Long.fromInt = function(value) {
    if (-128 <= value && value < 128) {
      var cachedObj = goog.math.Long.IntCache_[value];
      if (cachedObj) {
        return cachedObj;
      }
    }

    var obj = new goog.math.Long(value | 0, value < 0 ? -1 : 0);
    if (-128 <= value && value < 128) {
      goog.math.Long.IntCache_[value] = obj;
    }
    return obj;
  };


  /**
   * Returns a Long representing the given value, provided that it is a finite
   * number.  Otherwise, zero is returned.
   * @param {number} value The number in question.
   * @return {!goog.math.Long} The corresponding Long value.
   */
  goog.math.Long.fromNumber = function(value) {
    if (isNaN(value) || !isFinite(value)) {
      return goog.math.Long.ZERO;
    } else if (value <= -goog.math.Long.TWO_PWR_63_DBL_) {
      return goog.math.Long.MIN_VALUE;
    } else if (value + 1 >= goog.math.Long.TWO_PWR_63_DBL_) {
      return goog.math.Long.MAX_VALUE;
    } else if (value < 0) {
      return goog.math.Long.fromNumber(-value).negate();
    } else {
      return new goog.math.Long(
          (value % goog.math.Long.TWO_PWR_32_DBL_) | 0,
          (value / goog.math.Long.TWO_PWR_32_DBL_) | 0);
    }
  };


  /**
   * Returns a Long representing the 64-bit integer that comes by concatenating
   * the given high and low bits.  Each is assumed to use 32 bits.
   * @param {number} lowBits The low 32-bits.
   * @param {number} highBits The high 32-bits.
   * @return {!goog.math.Long} The corresponding Long value.
   */
  goog.math.Long.fromBits = function(lowBits, highBits) {
    return new goog.math.Long(lowBits, highBits);
  };


  /**
   * Returns a Long representation of the given string, written using the given
   * radix.
   * @param {string} str The textual representation of the Long.
   * @param {number=} opt_radix The radix in which the text is written.
   * @return {!goog.math.Long} The corresponding Long value.
   */
  goog.math.Long.fromString = function(str, opt_radix) {
    if (str.length == 0) {
      throw Error('number format error: empty string');
    }

    var radix = opt_radix || 10;
    if (radix < 2 || 36 < radix) {
      throw Error('radix out of range: ' + radix);
    }

    if (str.charAt(0) == '-') {
      return goog.math.Long.fromString(str.substring(1), radix).negate();
    } else if (str.indexOf('-') >= 0) {
      throw Error('number format error: interior "-" character: ' + str);
    }

    // Do several (8) digits each time through the loop, so as to
    // minimize the calls to the very expensive emulated div.
    var radixToPower = goog.math.Long.fromNumber(Math.pow(radix, 8));

    var result = goog.math.Long.ZERO;
    for (var i = 0; i < str.length; i += 8) {
      var size = Math.min(8, str.length - i);
      var value = parseInt(str.substring(i, i + size), radix);
      if (size < 8) {
        var power = goog.math.Long.fromNumber(Math.pow(radix, size));
        result = result.multiply(power).add(goog.math.Long.fromNumber(value));
      } else {
        result = result.multiply(radixToPower);
        result = result.add(goog.math.Long.fromNumber(value));
      }
    }
    return result;
  };


  // NOTE: the compiler should inline these constant values below and then remove
  // these variables, so there should be no runtime penalty for these.


  /**
   * Number used repeated below in calculations.  This must appear before the
   * first call to any from* function below.
   * @type {number}
   * @private
   */
  goog.math.Long.TWO_PWR_16_DBL_ = 1 << 16;


  /**
   * @type {number}
   * @private
   */
  goog.math.Long.TWO_PWR_24_DBL_ = 1 << 24;


  /**
   * @type {number}
   * @private
   */
  goog.math.Long.TWO_PWR_32_DBL_ =
      goog.math.Long.TWO_PWR_16_DBL_ * goog.math.Long.TWO_PWR_16_DBL_;


  /**
   * @type {number}
   * @private
   */
  goog.math.Long.TWO_PWR_31_DBL_ =
      goog.math.Long.TWO_PWR_32_DBL_ / 2;


  /**
   * @type {number}
   * @private
   */
  goog.math.Long.TWO_PWR_48_DBL_ =
      goog.math.Long.TWO_PWR_32_DBL_ * goog.math.Long.TWO_PWR_16_DBL_;


  /**
   * @type {number}
   * @private
   */
  goog.math.Long.TWO_PWR_64_DBL_ =
      goog.math.Long.TWO_PWR_32_DBL_ * goog.math.Long.TWO_PWR_32_DBL_;


  /**
   * @type {number}
   * @private
   */
  goog.math.Long.TWO_PWR_63_DBL_ =
      goog.math.Long.TWO_PWR_64_DBL_ / 2;


  /** @type {!goog.math.Long} */
  goog.math.Long.ZERO = goog.math.Long.fromInt(0);


  /** @type {!goog.math.Long} */
  goog.math.Long.ONE = goog.math.Long.fromInt(1);


  /** @type {!goog.math.Long} */
  goog.math.Long.NEG_ONE = goog.math.Long.fromInt(-1);


  /** @type {!goog.math.Long} */
  goog.math.Long.MAX_VALUE =
      goog.math.Long.fromBits(0xFFFFFFFF | 0, 0x7FFFFFFF | 0);


  /** @type {!goog.math.Long} */
  goog.math.Long.MIN_VALUE = goog.math.Long.fromBits(0, 0x80000000 | 0);


  /**
   * @type {!goog.math.Long}
   * @private
   */
  goog.math.Long.TWO_PWR_24_ = goog.math.Long.fromInt(1 << 24);


  /** @return {number} The value, assuming it is a 32-bit integer. */
  goog.math.Long.prototype.toInt = function() {
    return this.low_;
  };


  /** @return {number} The closest floating-point representation to this value. */
  goog.math.Long.prototype.toNumber = function() {
    return this.high_ * goog.math.Long.TWO_PWR_32_DBL_ +
           this.getLowBitsUnsigned();
  };


  /**
   * @param {number=} opt_radix The radix in which the text should be written.
   * @return {string} The textual representation of this value.
   */
  goog.math.Long.prototype.toString = function(opt_radix) {
    var radix = opt_radix || 10;
    if (radix < 2 || 36 < radix) {
      throw Error('radix out of range: ' + radix);
    }

    if (this.isZero()) {
      return '0';
    }

    if (this.isNegative()) {
      if (this.equals(goog.math.Long.MIN_VALUE)) {
        // We need to change the Long value before it can be negated, so we remove
        // the bottom-most digit in this base and then recurse to do the rest.
        var radixLong = goog.math.Long.fromNumber(radix);
        var div = this.div(radixLong);
        var rem = div.multiply(radixLong).subtract(this);
        return div.toString(radix) + rem.toInt().toString(radix);
      } else {
        return '-' + this.negate().toString(radix);
      }
    }

    // Do several (6) digits each time through the loop, so as to
    // minimize the calls to the very expensive emulated div.
    var radixToPower = goog.math.Long.fromNumber(Math.pow(radix, 6));

    var rem = this;
    var result = '';
    while (true) {
      var remDiv = rem.div(radixToPower);
      var intval = rem.subtract(remDiv.multiply(radixToPower)).toInt();
      var digits = intval.toString(radix);

      rem = remDiv;
      if (rem.isZero()) {
        return digits + result;
      } else {
        while (digits.length < 6) {
          digits = '0' + digits;
        }
        result = '' + digits + result;
      }
    }
  };


  /** @return {number} The high 32-bits as a signed value. */
  goog.math.Long.prototype.getHighBits = function() {
    return this.high_;
  };


  /** @return {number} The low 32-bits as a signed value. */
  goog.math.Long.prototype.getLowBits = function() {
    return this.low_;
  };


  /** @return {number} The low 32-bits as an unsigned value. */
  goog.math.Long.prototype.getLowBitsUnsigned = function() {
    return (this.low_ >= 0) ?
        this.low_ : goog.math.Long.TWO_PWR_32_DBL_ + this.low_;
  };


  /**
   * @return {number} Returns the number of bits needed to represent the absolute
   *     value of this Long.
   */
  goog.math.Long.prototype.getNumBitsAbs = function() {
    if (this.isNegative()) {
      if (this.equals(goog.math.Long.MIN_VALUE)) {
        return 64;
      } else {
        return this.negate().getNumBitsAbs();
      }
    } else {
      var val = this.high_ != 0 ? this.high_ : this.low_;
      for (var bit = 31; bit > 0; bit--) {
        if ((val & (1 << bit)) != 0) {
          break;
        }
      }
      return this.high_ != 0 ? bit + 33 : bit + 1;
    }
  };


  /** @return {boolean} Whether this value is zero. */
  goog.math.Long.prototype.isZero = function() {
    return this.high_ == 0 && this.low_ == 0;
  };


  /** @return {boolean} Whether this value is negative. */
  goog.math.Long.prototype.isNegative = function() {
    return this.high_ < 0;
  };


  /** @return {boolean} Whether this value is odd. */
  goog.math.Long.prototype.isOdd = function() {
    return (this.low_ & 1) == 1;
  };


  /**
   * @param {goog.math.Long} other Long to compare against.
   * @return {boolean} Whether this Long equals the other.
   */
  goog.math.Long.prototype.equals = function(other) {
    return (this.high_ == other.high_) && (this.low_ == other.low_);
  };


  /**
   * @param {goog.math.Long} other Long to compare against.
   * @return {boolean} Whether this Long does not equal the other.
   */
  goog.math.Long.prototype.notEquals = function(other) {
    return (this.high_ != other.high_) || (this.low_ != other.low_);
  };


  /**
   * @param {goog.math.Long} other Long to compare against.
   * @return {boolean} Whether this Long is less than the other.
   */
  goog.math.Long.prototype.lessThan = function(other) {
    return this.compare(other) < 0;
  };


  /**
   * @param {goog.math.Long} other Long to compare against.
   * @return {boolean} Whether this Long is less than or equal to the other.
   */
  goog.math.Long.prototype.lessThanOrEqual = function(other) {
    return this.compare(other) <= 0;
  };


  /**
   * @param {goog.math.Long} other Long to compare against.
   * @return {boolean} Whether this Long is greater than the other.
   */
  goog.math.Long.prototype.greaterThan = function(other) {
    return this.compare(other) > 0;
  };


  /**
   * @param {goog.math.Long} other Long to compare against.
   * @return {boolean} Whether this Long is greater than or equal to the other.
   */
  goog.math.Long.prototype.greaterThanOrEqual = function(other) {
    return this.compare(other) >= 0;
  };


  /**
   * Compares this Long with the given one.
   * @param {goog.math.Long} other Long to compare against.
   * @return {number} 0 if they are the same, 1 if the this is greater, and -1
   *     if the given one is greater.
   */
  goog.math.Long.prototype.compare = function(other) {
    if (this.equals(other)) {
      return 0;
    }

    var thisNeg = this.isNegative();
    var otherNeg = other.isNegative();
    if (thisNeg && !otherNeg) {
      return -1;
    }
    if (!thisNeg && otherNeg) {
      return 1;
    }

    // at this point, the signs are the same, so subtraction will not overflow
    if (this.subtract(other).isNegative()) {
      return -1;
    } else {
      return 1;
    }
  };


  /** @return {!goog.math.Long} The negation of this value. */
  goog.math.Long.prototype.negate = function() {
    if (this.equals(goog.math.Long.MIN_VALUE)) {
      return goog.math.Long.MIN_VALUE;
    } else {
      return this.not().add(goog.math.Long.ONE);
    }
  };


  /**
   * Returns the sum of this and the given Long.
   * @param {goog.math.Long} other Long to add to this one.
   * @return {!goog.math.Long} The sum of this and the given Long.
   */
  goog.math.Long.prototype.add = function(other) {
    // Divide each number into 4 chunks of 16 bits, and then sum the chunks.

    var a48 = this.high_ >>> 16;
    var a32 = this.high_ & 0xFFFF;
    var a16 = this.low_ >>> 16;
    var a00 = this.low_ & 0xFFFF;

    var b48 = other.high_ >>> 16;
    var b32 = other.high_ & 0xFFFF;
    var b16 = other.low_ >>> 16;
    var b00 = other.low_ & 0xFFFF;

    var c48 = 0, c32 = 0, c16 = 0, c00 = 0;
    c00 += a00 + b00;
    c16 += c00 >>> 16;
    c00 &= 0xFFFF;
    c16 += a16 + b16;
    c32 += c16 >>> 16;
    c16 &= 0xFFFF;
    c32 += a32 + b32;
    c48 += c32 >>> 16;
    c32 &= 0xFFFF;
    c48 += a48 + b48;
    c48 &= 0xFFFF;
    return goog.math.Long.fromBits((c16 << 16) | c00, (c48 << 16) | c32);
  };


  /**
   * Returns the difference of this and the given Long.
   * @param {goog.math.Long} other Long to subtract from this.
   * @return {!goog.math.Long} The difference of this and the given Long.
   */
  goog.math.Long.prototype.subtract = function(other) {
    return this.add(other.negate());
  };


  /**
   * Returns the product of this and the given long.
   * @param {goog.math.Long} other Long to multiply with this.
   * @return {!goog.math.Long} The product of this and the other.
   */
  goog.math.Long.prototype.multiply = function(other) {
    if (this.isZero()) {
      return goog.math.Long.ZERO;
    } else if (other.isZero()) {
      return goog.math.Long.ZERO;
    }

    if (this.equals(goog.math.Long.MIN_VALUE)) {
      return other.isOdd() ? goog.math.Long.MIN_VALUE : goog.math.Long.ZERO;
    } else if (other.equals(goog.math.Long.MIN_VALUE)) {
      return this.isOdd() ? goog.math.Long.MIN_VALUE : goog.math.Long.ZERO;
    }

    if (this.isNegative()) {
      if (other.isNegative()) {
        return this.negate().multiply(other.negate());
      } else {
        return this.negate().multiply(other).negate();
      }
    } else if (other.isNegative()) {
      return this.multiply(other.negate()).negate();
    }

    // If both longs are small, use float multiplication
    if (this.lessThan(goog.math.Long.TWO_PWR_24_) &&
        other.lessThan(goog.math.Long.TWO_PWR_24_)) {
      return goog.math.Long.fromNumber(this.toNumber() * other.toNumber());
    }

    // Divide each long into 4 chunks of 16 bits, and then add up 4x4 products.
    // We can skip products that would overflow.

    var a48 = this.high_ >>> 16;
    var a32 = this.high_ & 0xFFFF;
    var a16 = this.low_ >>> 16;
    var a00 = this.low_ & 0xFFFF;

    var b48 = other.high_ >>> 16;
    var b32 = other.high_ & 0xFFFF;
    var b16 = other.low_ >>> 16;
    var b00 = other.low_ & 0xFFFF;

    var c48 = 0, c32 = 0, c16 = 0, c00 = 0;
    c00 += a00 * b00;
    c16 += c00 >>> 16;
    c00 &= 0xFFFF;
    c16 += a16 * b00;
    c32 += c16 >>> 16;
    c16 &= 0xFFFF;
    c16 += a00 * b16;
    c32 += c16 >>> 16;
    c16 &= 0xFFFF;
    c32 += a32 * b00;
    c48 += c32 >>> 16;
    c32 &= 0xFFFF;
    c32 += a16 * b16;
    c48 += c32 >>> 16;
    c32 &= 0xFFFF;
    c32 += a00 * b32;
    c48 += c32 >>> 16;
    c32 &= 0xFFFF;
    c48 += a48 * b00 + a32 * b16 + a16 * b32 + a00 * b48;
    c48 &= 0xFFFF;
    return goog.math.Long.fromBits((c16 << 16) | c00, (c48 << 16) | c32);
  };


  /**
   * Returns this Long divided by the given one.
   * @param {goog.math.Long} other Long by which to divide.
   * @return {!goog.math.Long} This Long divided by the given one.
   */
  goog.math.Long.prototype.div = function(other) {
    if (other.isZero()) {
      throw Error('division by zero');
    } else if (this.isZero()) {
      return goog.math.Long.ZERO;
    }

    if (this.equals(goog.math.Long.MIN_VALUE)) {
      if (other.equals(goog.math.Long.ONE) ||
          other.equals(goog.math.Long.NEG_ONE)) {
        return goog.math.Long.MIN_VALUE;  // recall that -MIN_VALUE == MIN_VALUE
      } else if (other.equals(goog.math.Long.MIN_VALUE)) {
        return goog.math.Long.ONE;
      } else {
        // At this point, we have |other| >= 2, so |this/other| < |MIN_VALUE|.
        var halfThis = this.shiftRight(1);
        var approx = halfThis.div(other).shiftLeft(1);
        if (approx.equals(goog.math.Long.ZERO)) {
          return other.isNegative() ? goog.math.Long.ONE : goog.math.Long.NEG_ONE;
        } else {
          var rem = this.subtract(other.multiply(approx));
          var result = approx.add(rem.div(other));
          return result;
        }
      }
    } else if (other.equals(goog.math.Long.MIN_VALUE)) {
      return goog.math.Long.ZERO;
    }

    if (this.isNegative()) {
      if (other.isNegative()) {
        return this.negate().div(other.negate());
      } else {
        return this.negate().div(other).negate();
      }
    } else if (other.isNegative()) {
      return this.div(other.negate()).negate();
    }

    // Repeat the following until the remainder is less than other:  find a
    // floating-point that approximates remainder / other *from below*, add this
    // into the result, and subtract it from the remainder.  It is critical that
    // the approximate value is less than or equal to the real value so that the
    // remainder never becomes negative.
    var res = goog.math.Long.ZERO;
    var rem = this;
    while (rem.greaterThanOrEqual(other)) {
      // Approximate the result of division. This may be a little greater or
      // smaller than the actual value.
      var approx = Math.max(1, Math.floor(rem.toNumber() / other.toNumber()));

      // We will tweak the approximate result by changing it in the 48-th digit or
      // the smallest non-fractional digit, whichever is larger.
      var log2 = Math.ceil(Math.log(approx) / Math.LN2);
      var delta = (log2 <= 48) ? 1 : Math.pow(2, log2 - 48);

      // Decrease the approximation until it is smaller than the remainder.  Note
      // that if it is too large, the product overflows and is negative.
      var approxRes = goog.math.Long.fromNumber(approx);
      var approxRem = approxRes.multiply(other);
      while (approxRem.isNegative() || approxRem.greaterThan(rem)) {
        approx -= delta;
        approxRes = goog.math.Long.fromNumber(approx);
        approxRem = approxRes.multiply(other);
      }

      // We know the answer can't be zero... and actually, zero would cause
      // infinite recursion since we would make no progress.
      if (approxRes.isZero()) {
        approxRes = goog.math.Long.ONE;
      }

      res = res.add(approxRes);
      rem = rem.subtract(approxRem);
    }
    return res;
  };


  /**
   * Returns this Long modulo the given one.
   * @param {goog.math.Long} other Long by which to mod.
   * @return {!goog.math.Long} This Long modulo the given one.
   */
  goog.math.Long.prototype.modulo = function(other) {
    return this.subtract(this.div(other).multiply(other));
  };


  /** @return {!goog.math.Long} The bitwise-NOT of this value. */
  goog.math.Long.prototype.not = function() {
    return goog.math.Long.fromBits(~this.low_, ~this.high_);
  };


  /**
   * Returns the bitwise-AND of this Long and the given one.
   * @param {goog.math.Long} other The Long with which to AND.
   * @return {!goog.math.Long} The bitwise-AND of this and the other.
   */
  goog.math.Long.prototype.and = function(other) {
    return goog.math.Long.fromBits(this.low_ & other.low_,
                                   this.high_ & other.high_);
  };


  /**
   * Returns the bitwise-OR of this Long and the given one.
   * @param {goog.math.Long} other The Long with which to OR.
   * @return {!goog.math.Long} The bitwise-OR of this and the other.
   */
  goog.math.Long.prototype.or = function(other) {
    return goog.math.Long.fromBits(this.low_ | other.low_,
                                   this.high_ | other.high_);
  };


  /**
   * Returns the bitwise-XOR of this Long and the given one.
   * @param {goog.math.Long} other The Long with which to XOR.
   * @return {!goog.math.Long} The bitwise-XOR of this and the other.
   */
  goog.math.Long.prototype.xor = function(other) {
    return goog.math.Long.fromBits(this.low_ ^ other.low_,
                                   this.high_ ^ other.high_);
  };


  /**
   * Returns this Long with bits shifted to the left by the given amount.
   * @param {number} numBits The number of bits by which to shift.
   * @return {!goog.math.Long} This shifted to the left by the given amount.
   */
  goog.math.Long.prototype.shiftLeft = function(numBits) {
    numBits &= 63;
    if (numBits == 0) {
      return this;
    } else {
      var low = this.low_;
      if (numBits < 32) {
        var high = this.high_;
        return goog.math.Long.fromBits(
            low << numBits,
            (high << numBits) | (low >>> (32 - numBits)));
      } else {
        return goog.math.Long.fromBits(0, low << (numBits - 32));
      }
    }
  };


  /**
   * Returns this Long with bits shifted to the right by the given amount.
   * @param {number} numBits The number of bits by which to shift.
   * @return {!goog.math.Long} This shifted to the right by the given amount.
   */
  goog.math.Long.prototype.shiftRight = function(numBits) {
    numBits &= 63;
    if (numBits == 0) {
      return this;
    } else {
      var high = this.high_;
      if (numBits < 32) {
        var low = this.low_;
        return goog.math.Long.fromBits(
            (low >>> numBits) | (high << (32 - numBits)),
            high >> numBits);
      } else {
        return goog.math.Long.fromBits(
            high >> (numBits - 32),
            high >= 0 ? 0 : -1);
      }
    }
  };


  /**
   * Returns this Long with bits shifted to the right by the given amount, with
   * the new top bits matching the current sign bit.
   * @param {number} numBits The number of bits by which to shift.
   * @return {!goog.math.Long} This shifted to the right by the given amount, with
   *     zeros placed into the new leading bits.
   */
  goog.math.Long.prototype.shiftRightUnsigned = function(numBits) {
    numBits &= 63;
    if (numBits == 0) {
      return this;
    } else {
      var high = this.high_;
      if (numBits < 32) {
        var low = this.low_;
        return goog.math.Long.fromBits(
            (low >>> numBits) | (high << (32 - numBits)),
            high >>> numBits);
      } else if (numBits == 32) {
        return goog.math.Long.fromBits(high, 0);
      } else {
        return goog.math.Long.fromBits(high >>> (numBits - 32), 0);
      }
    }
  };

  //======= begin jsbn =======

  var navigator = { appName: 'Modern Browser' }; // polyfill a little

  // Copyright (c) 2005  Tom Wu
  // All Rights Reserved.
  // http://www-cs-students.stanford.edu/~tjw/jsbn/

  /*
   * Copyright (c) 2003-2005  Tom Wu
   * All Rights Reserved.
   *
   * Permission is hereby granted, free of charge, to any person obtaining
   * a copy of this software and associated documentation files (the
   * "Software"), to deal in the Software without restriction, including
   * without limitation the rights to use, copy, modify, merge, publish,
   * distribute, sublicense, and/or sell copies of the Software, and to
   * permit persons to whom the Software is furnished to do so, subject to
   * the following conditions:
   *
   * The above copyright notice and this permission notice shall be
   * included in all copies or substantial portions of the Software.
   *
   * THE SOFTWARE IS PROVIDED "AS-IS" AND WITHOUT WARRANTY OF ANY KIND, 
   * EXPRESS, IMPLIED OR OTHERWISE, INCLUDING WITHOUT LIMITATION, ANY 
   * WARRANTY OF MERCHANTABILITY OR FITNESS FOR A PARTICULAR PURPOSE.  
   *
   * IN NO EVENT SHALL TOM WU BE LIABLE FOR ANY SPECIAL, INCIDENTAL,
   * INDIRECT OR CONSEQUENTIAL DAMAGES OF ANY KIND, OR ANY DAMAGES WHATSOEVER
   * RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER OR NOT ADVISED OF
   * THE POSSIBILITY OF DAMAGE, AND ON ANY THEORY OF LIABILITY, ARISING OUT
   * OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
   *
   * In addition, the following condition applies:
   *
   * All redistributions must retain an intact copy of this copyright notice
   * and disclaimer.
   */

  // Basic JavaScript BN library - subset useful for RSA encryption.

  // Bits per digit
  var dbits;

  // JavaScript engine analysis
  var canary = 0xdeadbeefcafe;
  var j_lm = ((canary&0xffffff)==0xefcafe);

  // (public) Constructor
  function BigInteger(a,b,c) {
    if(a != null)
      if("number" == typeof a) this.fromNumber(a,b,c);
      else if(b == null && "string" != typeof a) this.fromString(a,256);
      else this.fromString(a,b);
  }

  // return new, unset BigInteger
  function nbi() { return new BigInteger(null); }

  // am: Compute w_j += (x*this_i), propagate carries,
  // c is initial carry, returns final carry.
  // c < 3*dvalue, x < 2*dvalue, this_i < dvalue
  // We need to select the fastest one that works in this environment.

  // am1: use a single mult and divide to get the high bits,
  // max digit bits should be 26 because
  // max internal value = 2*dvalue^2-2*dvalue (< 2^53)
  function am1(i,x,w,j,c,n) {
    while(--n >= 0) {
      var v = x*this[i++]+w[j]+c;
      c = Math.floor(v/0x4000000);
      w[j++] = v&0x3ffffff;
    }
    return c;
  }
  // am2 avoids a big mult-and-extract completely.
  // Max digit bits should be <= 30 because we do bitwise ops
  // on values up to 2*hdvalue^2-hdvalue-1 (< 2^31)
  function am2(i,x,w,j,c,n) {
    var xl = x&0x7fff, xh = x>>15;
    while(--n >= 0) {
      var l = this[i]&0x7fff;
      var h = this[i++]>>15;
      var m = xh*l+h*xl;
      l = xl*l+((m&0x7fff)<<15)+w[j]+(c&0x3fffffff);
      c = (l>>>30)+(m>>>15)+xh*h+(c>>>30);
      w[j++] = l&0x3fffffff;
    }
    return c;
  }
  // Alternately, set max digit bits to 28 since some
  // browsers slow down when dealing with 32-bit numbers.
  function am3(i,x,w,j,c,n) {
    var xl = x&0x3fff, xh = x>>14;
    while(--n >= 0) {
      var l = this[i]&0x3fff;
      var h = this[i++]>>14;
      var m = xh*l+h*xl;
      l = xl*l+((m&0x3fff)<<14)+w[j]+c;
      c = (l>>28)+(m>>14)+xh*h;
      w[j++] = l&0xfffffff;
    }
    return c;
  }
  if(j_lm && (navigator.appName == "Microsoft Internet Explorer")) {
    BigInteger.prototype.am = am2;
    dbits = 30;
  }
  else if(j_lm && (navigator.appName != "Netscape")) {
    BigInteger.prototype.am = am1;
    dbits = 26;
  }
  else { // Mozilla/Netscape seems to prefer am3
    BigInteger.prototype.am = am3;
    dbits = 28;
  }

  BigInteger.prototype.DB = dbits;
  BigInteger.prototype.DM = ((1<<dbits)-1);
  BigInteger.prototype.DV = (1<<dbits);

  var BI_FP = 52;
  BigInteger.prototype.FV = Math.pow(2,BI_FP);
  BigInteger.prototype.F1 = BI_FP-dbits;
  BigInteger.prototype.F2 = 2*dbits-BI_FP;

  // Digit conversions
  var BI_RM = "0123456789abcdefghijklmnopqrstuvwxyz";
  var BI_RC = new Array();
  var rr,vv;
  rr = "0".charCodeAt(0);
  for(vv = 0; vv <= 9; ++vv) BI_RC[rr++] = vv;
  rr = "a".charCodeAt(0);
  for(vv = 10; vv < 36; ++vv) BI_RC[rr++] = vv;
  rr = "A".charCodeAt(0);
  for(vv = 10; vv < 36; ++vv) BI_RC[rr++] = vv;

  function int2char(n) { return BI_RM.charAt(n); }
  function intAt(s,i) {
    var c = BI_RC[s.charCodeAt(i)];
    return (c==null)?-1:c;
  }

  // (protected) copy this to r
  function bnpCopyTo(r) {
    for(var i = this.t-1; i >= 0; --i) r[i] = this[i];
    r.t = this.t;
    r.s = this.s;
  }

  // (protected) set from integer value x, -DV <= x < DV
  function bnpFromInt(x) {
    this.t = 1;
    this.s = (x<0)?-1:0;
    if(x > 0) this[0] = x;
    else if(x < -1) this[0] = x+DV;
    else this.t = 0;
  }

  // return bigint initialized to value
  function nbv(i) { var r = nbi(); r.fromInt(i); return r; }

  // (protected) set from string and radix
  function bnpFromString(s,b) {
    var k;
    if(b == 16) k = 4;
    else if(b == 8) k = 3;
    else if(b == 256) k = 8; // byte array
    else if(b == 2) k = 1;
    else if(b == 32) k = 5;
    else if(b == 4) k = 2;
    else { this.fromRadix(s,b); return; }
    this.t = 0;
    this.s = 0;
    var i = s.length, mi = false, sh = 0;
    while(--i >= 0) {
      var x = (k==8)?s[i]&0xff:intAt(s,i);
      if(x < 0) {
        if(s.charAt(i) == "-") mi = true;
        continue;
      }
      mi = false;
      if(sh == 0)
        this[this.t++] = x;
      else if(sh+k > this.DB) {
        this[this.t-1] |= (x&((1<<(this.DB-sh))-1))<<sh;
        this[this.t++] = (x>>(this.DB-sh));
      }
      else
        this[this.t-1] |= x<<sh;
      sh += k;
      if(sh >= this.DB) sh -= this.DB;
    }
    if(k == 8 && (s[0]&0x80) != 0) {
      this.s = -1;
      if(sh > 0) this[this.t-1] |= ((1<<(this.DB-sh))-1)<<sh;
    }
    this.clamp();
    if(mi) BigInteger.ZERO.subTo(this,this);
  }

  // (protected) clamp off excess high words
  function bnpClamp() {
    var c = this.s&this.DM;
    while(this.t > 0 && this[this.t-1] == c) --this.t;
  }

  // (public) return string representation in given radix
  function bnToString(b) {
    if(this.s < 0) return "-"+this.negate().toString(b);
    var k;
    if(b == 16) k = 4;
    else if(b == 8) k = 3;
    else if(b == 2) k = 1;
    else if(b == 32) k = 5;
    else if(b == 4) k = 2;
    else return this.toRadix(b);
    var km = (1<<k)-1, d, m = false, r = "", i = this.t;
    var p = this.DB-(i*this.DB)%k;
    if(i-- > 0) {
      if(p < this.DB && (d = this[i]>>p) > 0) { m = true; r = int2char(d); }
      while(i >= 0) {
        if(p < k) {
          d = (this[i]&((1<<p)-1))<<(k-p);
          d |= this[--i]>>(p+=this.DB-k);
        }
        else {
          d = (this[i]>>(p-=k))&km;
          if(p <= 0) { p += this.DB; --i; }
        }
        if(d > 0) m = true;
        if(m) r += int2char(d);
      }
    }
    return m?r:"0";
  }

  // (public) -this
  function bnNegate() { var r = nbi(); BigInteger.ZERO.subTo(this,r); return r; }

  // (public) |this|
  function bnAbs() { return (this.s<0)?this.negate():this; }

  // (public) return + if this > a, - if this < a, 0 if equal
  function bnCompareTo(a) {
    var r = this.s-a.s;
    if(r != 0) return r;
    var i = this.t;
    r = i-a.t;
    if(r != 0) return (this.s<0)?-r:r;
    while(--i >= 0) if((r=this[i]-a[i]) != 0) return r;
    return 0;
  }

  // returns bit length of the integer x
  function nbits(x) {
    var r = 1, t;
    if((t=x>>>16) != 0) { x = t; r += 16; }
    if((t=x>>8) != 0) { x = t; r += 8; }
    if((t=x>>4) != 0) { x = t; r += 4; }
    if((t=x>>2) != 0) { x = t; r += 2; }
    if((t=x>>1) != 0) { x = t; r += 1; }
    return r;
  }

  // (public) return the number of bits in "this"
  function bnBitLength() {
    if(this.t <= 0) return 0;
    return this.DB*(this.t-1)+nbits(this[this.t-1]^(this.s&this.DM));
  }

  // (protected) r = this << n*DB
  function bnpDLShiftTo(n,r) {
    var i;
    for(i = this.t-1; i >= 0; --i) r[i+n] = this[i];
    for(i = n-1; i >= 0; --i) r[i] = 0;
    r.t = this.t+n;
    r.s = this.s;
  }

  // (protected) r = this >> n*DB
  function bnpDRShiftTo(n,r) {
    for(var i = n; i < this.t; ++i) r[i-n] = this[i];
    r.t = Math.max(this.t-n,0);
    r.s = this.s;
  }

  // (protected) r = this << n
  function bnpLShiftTo(n,r) {
    var bs = n%this.DB;
    var cbs = this.DB-bs;
    var bm = (1<<cbs)-1;
    var ds = Math.floor(n/this.DB), c = (this.s<<bs)&this.DM, i;
    for(i = this.t-1; i >= 0; --i) {
      r[i+ds+1] = (this[i]>>cbs)|c;
      c = (this[i]&bm)<<bs;
    }
    for(i = ds-1; i >= 0; --i) r[i] = 0;
    r[ds] = c;
    r.t = this.t+ds+1;
    r.s = this.s;
    r.clamp();
  }

  // (protected) r = this >> n
  function bnpRShiftTo(n,r) {
    r.s = this.s;
    var ds = Math.floor(n/this.DB);
    if(ds >= this.t) { r.t = 0; return; }
    var bs = n%this.DB;
    var cbs = this.DB-bs;
    var bm = (1<<bs)-1;
    r[0] = this[ds]>>bs;
    for(var i = ds+1; i < this.t; ++i) {
      r[i-ds-1] |= (this[i]&bm)<<cbs;
      r[i-ds] = this[i]>>bs;
    }
    if(bs > 0) r[this.t-ds-1] |= (this.s&bm)<<cbs;
    r.t = this.t-ds;
    r.clamp();
  }

  // (protected) r = this - a
  function bnpSubTo(a,r) {
    var i = 0, c = 0, m = Math.min(a.t,this.t);
    while(i < m) {
      c += this[i]-a[i];
      r[i++] = c&this.DM;
      c >>= this.DB;
    }
    if(a.t < this.t) {
      c -= a.s;
      while(i < this.t) {
        c += this[i];
        r[i++] = c&this.DM;
        c >>= this.DB;
      }
      c += this.s;
    }
    else {
      c += this.s;
      while(i < a.t) {
        c -= a[i];
        r[i++] = c&this.DM;
        c >>= this.DB;
      }
      c -= a.s;
    }
    r.s = (c<0)?-1:0;
    if(c < -1) r[i++] = this.DV+c;
    else if(c > 0) r[i++] = c;
    r.t = i;
    r.clamp();
  }

  // (protected) r = this * a, r != this,a (HAC 14.12)
  // "this" should be the larger one if appropriate.
  function bnpMultiplyTo(a,r) {
    var x = this.abs(), y = a.abs();
    var i = x.t;
    r.t = i+y.t;
    while(--i >= 0) r[i] = 0;
    for(i = 0; i < y.t; ++i) r[i+x.t] = x.am(0,y[i],r,i,0,x.t);
    r.s = 0;
    r.clamp();
    if(this.s != a.s) BigInteger.ZERO.subTo(r,r);
  }

  // (protected) r = this^2, r != this (HAC 14.16)
  function bnpSquareTo(r) {
    var x = this.abs();
    var i = r.t = 2*x.t;
    while(--i >= 0) r[i] = 0;
    for(i = 0; i < x.t-1; ++i) {
      var c = x.am(i,x[i],r,2*i,0,1);
      if((r[i+x.t]+=x.am(i+1,2*x[i],r,2*i+1,c,x.t-i-1)) >= x.DV) {
        r[i+x.t] -= x.DV;
        r[i+x.t+1] = 1;
      }
    }
    if(r.t > 0) r[r.t-1] += x.am(i,x[i],r,2*i,0,1);
    r.s = 0;
    r.clamp();
  }

  // (protected) divide this by m, quotient and remainder to q, r (HAC 14.20)
  // r != q, this != m.  q or r may be null.
  function bnpDivRemTo(m,q,r) {
    var pm = m.abs();
    if(pm.t <= 0) return;
    var pt = this.abs();
    if(pt.t < pm.t) {
      if(q != null) q.fromInt(0);
      if(r != null) this.copyTo(r);
      return;
    }
    if(r == null) r = nbi();
    var y = nbi(), ts = this.s, ms = m.s;
    var nsh = this.DB-nbits(pm[pm.t-1]);	// normalize modulus
    if(nsh > 0) { pm.lShiftTo(nsh,y); pt.lShiftTo(nsh,r); }
    else { pm.copyTo(y); pt.copyTo(r); }
    var ys = y.t;
    var y0 = y[ys-1];
    if(y0 == 0) return;
    var yt = y0*(1<<this.F1)+((ys>1)?y[ys-2]>>this.F2:0);
    var d1 = this.FV/yt, d2 = (1<<this.F1)/yt, e = 1<<this.F2;
    var i = r.t, j = i-ys, t = (q==null)?nbi():q;
    y.dlShiftTo(j,t);
    if(r.compareTo(t) >= 0) {
      r[r.t++] = 1;
      r.subTo(t,r);
    }
    BigInteger.ONE.dlShiftTo(ys,t);
    t.subTo(y,y);	// "negative" y so we can replace sub with am later
    while(y.t < ys) y[y.t++] = 0;
    while(--j >= 0) {
      // Estimate quotient digit
      var qd = (r[--i]==y0)?this.DM:Math.floor(r[i]*d1+(r[i-1]+e)*d2);
      if((r[i]+=y.am(0,qd,r,j,0,ys)) < qd) {	// Try it out
        y.dlShiftTo(j,t);
        r.subTo(t,r);
        while(r[i] < --qd) r.subTo(t,r);
      }
    }
    if(q != null) {
      r.drShiftTo(ys,q);
      if(ts != ms) BigInteger.ZERO.subTo(q,q);
    }
    r.t = ys;
    r.clamp();
    if(nsh > 0) r.rShiftTo(nsh,r);	// Denormalize remainder
    if(ts < 0) BigInteger.ZERO.subTo(r,r);
  }

  // (public) this mod a
  function bnMod(a) {
    var r = nbi();
    this.abs().divRemTo(a,null,r);
    if(this.s < 0 && r.compareTo(BigInteger.ZERO) > 0) a.subTo(r,r);
    return r;
  }

  // Modular reduction using "classic" algorithm
  function Classic(m) { this.m = m; }
  function cConvert(x) {
    if(x.s < 0 || x.compareTo(this.m) >= 0) return x.mod(this.m);
    else return x;
  }
  function cRevert(x) { return x; }
  function cReduce(x) { x.divRemTo(this.m,null,x); }
  function cMulTo(x,y,r) { x.multiplyTo(y,r); this.reduce(r); }
  function cSqrTo(x,r) { x.squareTo(r); this.reduce(r); }

  Classic.prototype.convert = cConvert;
  Classic.prototype.revert = cRevert;
  Classic.prototype.reduce = cReduce;
  Classic.prototype.mulTo = cMulTo;
  Classic.prototype.sqrTo = cSqrTo;

  // (protected) return "-1/this % 2^DB"; useful for Mont. reduction
  // justification:
  //         xy == 1 (mod m)
  //         xy =  1+km
  //   xy(2-xy) = (1+km)(1-km)
  // x[y(2-xy)] = 1-k^2m^2
  // x[y(2-xy)] == 1 (mod m^2)
  // if y is 1/x mod m, then y(2-xy) is 1/x mod m^2
  // should reduce x and y(2-xy) by m^2 at each step to keep size bounded.
  // JS multiply "overflows" differently from C/C++, so care is needed here.
  function bnpInvDigit() {
    if(this.t < 1) return 0;
    var x = this[0];
    if((x&1) == 0) return 0;
    var y = x&3;		// y == 1/x mod 2^2
    y = (y*(2-(x&0xf)*y))&0xf;	// y == 1/x mod 2^4
    y = (y*(2-(x&0xff)*y))&0xff;	// y == 1/x mod 2^8
    y = (y*(2-(((x&0xffff)*y)&0xffff)))&0xffff;	// y == 1/x mod 2^16
    // last step - calculate inverse mod DV directly;
    // assumes 16 < DB <= 32 and assumes ability to handle 48-bit ints
    y = (y*(2-x*y%this.DV))%this.DV;		// y == 1/x mod 2^dbits
    // we really want the negative inverse, and -DV < y < DV
    return (y>0)?this.DV-y:-y;
  }

  // Montgomery reduction
  function Montgomery(m) {
    this.m = m;
    this.mp = m.invDigit();
    this.mpl = this.mp&0x7fff;
    this.mph = this.mp>>15;
    this.um = (1<<(m.DB-15))-1;
    this.mt2 = 2*m.t;
  }

  // xR mod m
  function montConvert(x) {
    var r = nbi();
    x.abs().dlShiftTo(this.m.t,r);
    r.divRemTo(this.m,null,r);
    if(x.s < 0 && r.compareTo(BigInteger.ZERO) > 0) this.m.subTo(r,r);
    return r;
  }

  // x/R mod m
  function montRevert(x) {
    var r = nbi();
    x.copyTo(r);
    this.reduce(r);
    return r;
  }

  // x = x/R mod m (HAC 14.32)
  function montReduce(x) {
    while(x.t <= this.mt2)	// pad x so am has enough room later
      x[x.t++] = 0;
    for(var i = 0; i < this.m.t; ++i) {
      // faster way of calculating u0 = x[i]*mp mod DV
      var j = x[i]&0x7fff;
      var u0 = (j*this.mpl+(((j*this.mph+(x[i]>>15)*this.mpl)&this.um)<<15))&x.DM;
      // use am to combine the multiply-shift-add into one call
      j = i+this.m.t;
      x[j] += this.m.am(0,u0,x,i,0,this.m.t);
      // propagate carry
      while(x[j] >= x.DV) { x[j] -= x.DV; x[++j]++; }
    }
    x.clamp();
    x.drShiftTo(this.m.t,x);
    if(x.compareTo(this.m) >= 0) x.subTo(this.m,x);
  }

  // r = "x^2/R mod m"; x != r
  function montSqrTo(x,r) { x.squareTo(r); this.reduce(r); }

  // r = "xy/R mod m"; x,y != r
  function montMulTo(x,y,r) { x.multiplyTo(y,r); this.reduce(r); }

  Montgomery.prototype.convert = montConvert;
  Montgomery.prototype.revert = montRevert;
  Montgomery.prototype.reduce = montReduce;
  Montgomery.prototype.mulTo = montMulTo;
  Montgomery.prototype.sqrTo = montSqrTo;

  // (protected) true iff this is even
  function bnpIsEven() { return ((this.t>0)?(this[0]&1):this.s) == 0; }

  // (protected) this^e, e < 2^32, doing sqr and mul with "r" (HAC 14.79)
  function bnpExp(e,z) {
    if(e > 0xffffffff || e < 1) return BigInteger.ONE;
    var r = nbi(), r2 = nbi(), g = z.convert(this), i = nbits(e)-1;
    g.copyTo(r);
    while(--i >= 0) {
      z.sqrTo(r,r2);
      if((e&(1<<i)) > 0) z.mulTo(r2,g,r);
      else { var t = r; r = r2; r2 = t; }
    }
    return z.revert(r);
  }

  // (public) this^e % m, 0 <= e < 2^32
  function bnModPowInt(e,m) {
    var z;
    if(e < 256 || m.isEven()) z = new Classic(m); else z = new Montgomery(m);
    return this.exp(e,z);
  }

  // protected
  BigInteger.prototype.copyTo = bnpCopyTo;
  BigInteger.prototype.fromInt = bnpFromInt;
  BigInteger.prototype.fromString = bnpFromString;
  BigInteger.prototype.clamp = bnpClamp;
  BigInteger.prototype.dlShiftTo = bnpDLShiftTo;
  BigInteger.prototype.drShiftTo = bnpDRShiftTo;
  BigInteger.prototype.lShiftTo = bnpLShiftTo;
  BigInteger.prototype.rShiftTo = bnpRShiftTo;
  BigInteger.prototype.subTo = bnpSubTo;
  BigInteger.prototype.multiplyTo = bnpMultiplyTo;
  BigInteger.prototype.squareTo = bnpSquareTo;
  BigInteger.prototype.divRemTo = bnpDivRemTo;
  BigInteger.prototype.invDigit = bnpInvDigit;
  BigInteger.prototype.isEven = bnpIsEven;
  BigInteger.prototype.exp = bnpExp;

  // public
  BigInteger.prototype.toString = bnToString;
  BigInteger.prototype.negate = bnNegate;
  BigInteger.prototype.abs = bnAbs;
  BigInteger.prototype.compareTo = bnCompareTo;
  BigInteger.prototype.bitLength = bnBitLength;
  BigInteger.prototype.mod = bnMod;
  BigInteger.prototype.modPowInt = bnModPowInt;

  // "constants"
  BigInteger.ZERO = nbv(0);
  BigInteger.ONE = nbv(1);

  // jsbn2 stuff

  // (protected) convert from radix string
  function bnpFromRadix(s,b) {
    this.fromInt(0);
    if(b == null) b = 10;
    var cs = this.chunkSize(b);
    var d = Math.pow(b,cs), mi = false, j = 0, w = 0;
    for(var i = 0; i < s.length; ++i) {
      var x = intAt(s,i);
      if(x < 0) {
        if(s.charAt(i) == "-" && this.signum() == 0) mi = true;
        continue;
      }
      w = b*w+x;
      if(++j >= cs) {
        this.dMultiply(d);
        this.dAddOffset(w,0);
        j = 0;
        w = 0;
      }
    }
    if(j > 0) {
      this.dMultiply(Math.pow(b,j));
      this.dAddOffset(w,0);
    }
    if(mi) BigInteger.ZERO.subTo(this,this);
  }

  // (protected) return x s.t. r^x < DV
  function bnpChunkSize(r) { return Math.floor(Math.LN2*this.DB/Math.log(r)); }

  // (public) 0 if this == 0, 1 if this > 0
  function bnSigNum() {
    if(this.s < 0) return -1;
    else if(this.t <= 0 || (this.t == 1 && this[0] <= 0)) return 0;
    else return 1;
  }

  // (protected) this *= n, this >= 0, 1 < n < DV
  function bnpDMultiply(n) {
    this[this.t] = this.am(0,n-1,this,0,0,this.t);
    ++this.t;
    this.clamp();
  }

  // (protected) this += n << w words, this >= 0
  function bnpDAddOffset(n,w) {
    if(n == 0) return;
    while(this.t <= w) this[this.t++] = 0;
    this[w] += n;
    while(this[w] >= this.DV) {
      this[w] -= this.DV;
      if(++w >= this.t) this[this.t++] = 0;
      ++this[w];
    }
  }

  // (protected) convert to radix string
  function bnpToRadix(b) {
    if(b == null) b = 10;
    if(this.signum() == 0 || b < 2 || b > 36) return "0";
    var cs = this.chunkSize(b);
    var a = Math.pow(b,cs);
    var d = nbv(a), y = nbi(), z = nbi(), r = "";
    this.divRemTo(d,y,z);
    while(y.signum() > 0) {
      r = (a+z.intValue()).toString(b).substr(1) + r;
      y.divRemTo(d,y,z);
    }
    return z.intValue().toString(b) + r;
  }

  // (public) return value as integer
  function bnIntValue() {
    if(this.s < 0) {
      if(this.t == 1) return this[0]-this.DV;
      else if(this.t == 0) return -1;
    }
    else if(this.t == 1) return this[0];
    else if(this.t == 0) return 0;
    // assumes 16 < DB < 32
    return ((this[1]&((1<<(32-this.DB))-1))<<this.DB)|this[0];
  }

  // (protected) r = this + a
  function bnpAddTo(a,r) {
    var i = 0, c = 0, m = Math.min(a.t,this.t);
    while(i < m) {
      c += this[i]+a[i];
      r[i++] = c&this.DM;
      c >>= this.DB;
    }
    if(a.t < this.t) {
      c += a.s;
      while(i < this.t) {
        c += this[i];
        r[i++] = c&this.DM;
        c >>= this.DB;
      }
      c += this.s;
    }
    else {
      c += this.s;
      while(i < a.t) {
        c += a[i];
        r[i++] = c&this.DM;
        c >>= this.DB;
      }
      c += a.s;
    }
    r.s = (c<0)?-1:0;
    if(c > 0) r[i++] = c;
    else if(c < -1) r[i++] = this.DV+c;
    r.t = i;
    r.clamp();
  }

  BigInteger.prototype.fromRadix = bnpFromRadix;
  BigInteger.prototype.chunkSize = bnpChunkSize;
  BigInteger.prototype.signum = bnSigNum;
  BigInteger.prototype.dMultiply = bnpDMultiply;
  BigInteger.prototype.dAddOffset = bnpDAddOffset;
  BigInteger.prototype.toRadix = bnpToRadix;
  BigInteger.prototype.intValue = bnIntValue;
  BigInteger.prototype.addTo = bnpAddTo;

  //======= end jsbn =======

  // Emscripten wrapper
  var Wrapper = {
    abs: function(l, h) {
      var x = new goog.math.Long(l, h);
      var ret;
      if (x.isNegative()) {
        ret = x.negate();
      } else {
        ret = x;
      }
      HEAP32[tempDoublePtr>>2] = ret.low_;
      HEAP32[tempDoublePtr+4>>2] = ret.high_;
    },
    ensureTemps: function() {
      if (Wrapper.ensuredTemps) return;
      Wrapper.ensuredTemps = true;
      Wrapper.two32 = new BigInteger();
      Wrapper.two32.fromString('4294967296', 10);
      Wrapper.two64 = new BigInteger();
      Wrapper.two64.fromString('18446744073709551616', 10);
      Wrapper.temp1 = new BigInteger();
      Wrapper.temp2 = new BigInteger();
    },
    lh2bignum: function(l, h) {
      var a = new BigInteger();
      a.fromString(h.toString(), 10);
      var b = new BigInteger();
      a.multiplyTo(Wrapper.two32, b);
      var c = new BigInteger();
      c.fromString(l.toString(), 10);
      var d = new BigInteger();
      c.addTo(b, d);
      return d;
    },
    stringify: function(l, h, unsigned) {
      var ret = new goog.math.Long(l, h).toString();
      if (unsigned && ret[0] == '-') {
        // unsign slowly using jsbn bignums
        Wrapper.ensureTemps();
        var bignum = new BigInteger();
        bignum.fromString(ret, 10);
        ret = new BigInteger();
        Wrapper.two64.addTo(bignum, ret);
        ret = ret.toString(10);
      }
      return ret;
    },
    fromString: function(str, base, min, max, unsigned) {
      Wrapper.ensureTemps();
      var bignum = new BigInteger();
      bignum.fromString(str, base);
      var bigmin = new BigInteger();
      bigmin.fromString(min, 10);
      var bigmax = new BigInteger();
      bigmax.fromString(max, 10);
      if (unsigned && bignum.compareTo(BigInteger.ZERO) < 0) {
        var temp = new BigInteger();
        bignum.addTo(Wrapper.two64, temp);
        bignum = temp;
      }
      var error = false;
      if (bignum.compareTo(bigmin) < 0) {
        bignum = bigmin;
        error = true;
      } else if (bignum.compareTo(bigmax) > 0) {
        bignum = bigmax;
        error = true;
      }
      var ret = goog.math.Long.fromString(bignum.toString()); // min-max checks should have clamped this to a range goog.math.Long can handle well
      HEAP32[tempDoublePtr>>2] = ret.low_;
      HEAP32[tempDoublePtr+4>>2] = ret.high_;
      if (error) throw 'range error';
    }
  };
  return Wrapper;
})();

//======= end closure i64 code =======



// === Auto-generated postamble setup entry stuff ===

if (memoryInitializer) {
  if (ENVIRONMENT_IS_NODE || ENVIRONMENT_IS_SHELL) {
    var data = Module['readBinary'](memoryInitializer);
    HEAPU8.set(data, STATIC_BASE);
  } else {
    addRunDependency('memory initializer');
    Browser.asyncLoad(memoryInitializer, function(data) {
      HEAPU8.set(data, STATIC_BASE);
      removeRunDependency('memory initializer');
    }, function(data) {
      throw 'could not load memory initializer ' + memoryInitializer;
    });
  }
}

function ExitStatus(status) {
  this.name = "ExitStatus";
  this.message = "Program terminated with exit(" + status + ")";
  this.status = status;
};
ExitStatus.prototype = new Error();
ExitStatus.prototype.constructor = ExitStatus;

var initialStackTop;
var preloadStartTime = null;
var calledMain = false;

dependenciesFulfilled = function runCaller() {
  // If run has never been called, and we should call run (INVOKE_RUN is true, and Module.noInitialRun is not false)
  if (!Module['calledRun'] && shouldRunNow) run();
  if (!Module['calledRun']) dependenciesFulfilled = runCaller; // try this again later, after new deps are fulfilled
}

Module['callMain'] = Module.callMain = function callMain(args) {
  assert(runDependencies == 0, 'cannot call main when async dependencies remain! (listen on __ATMAIN__)');
  assert(__ATPRERUN__.length == 0, 'cannot call main when preRun functions remain to be called');

  args = args || [];

  if (ENVIRONMENT_IS_WEB && preloadStartTime !== null) {
    Module.printErr('preload time: ' + (Date.now() - preloadStartTime) + ' ms');
  }

  ensureInitRuntime();

  var argc = args.length+1;
  function pad() {
    for (var i = 0; i < 4-1; i++) {
      argv.push(0);
    }
  }
  var argv = [allocate(intArrayFromString("/bin/this.program"), 'i8', ALLOC_NORMAL) ];
  pad();
  for (var i = 0; i < argc-1; i = i + 1) {
    argv.push(allocate(intArrayFromString(args[i]), 'i8', ALLOC_NORMAL));
    pad();
  }
  argv.push(0);
  argv = allocate(argv, 'i32', ALLOC_NORMAL);

  initialStackTop = STACKTOP;

  try {

    var ret = Module['_main'](argc, argv, 0);


    // if we're not running an evented main loop, it's time to exit
    if (!Module['noExitRuntime']) {
      exit(ret);
    }
  }
  catch(e) {
    if (e instanceof ExitStatus) {
      // exit() throws this once it's done to make sure execution
      // has been stopped completely
      return;
    } else if (e == 'SimulateInfiniteLoop') {
      // running an evented main loop, don't immediately exit
      Module['noExitRuntime'] = true;
      return;
    } else {
      if (e && typeof e === 'object' && e.stack) Module.printErr('exception thrown: ' + [e, e.stack]);
      throw e;
    }
  } finally {
    calledMain = true;
  }
}




function run(args) {
  args = args || Module['arguments'];

  if (preloadStartTime === null) preloadStartTime = Date.now();

  if (runDependencies > 0) {
    Module.printErr('run() called, but dependencies remain, so not running');
    return;
  }

  preRun();

  if (runDependencies > 0) return; // a preRun added a dependency, run will be called later
  if (Module['calledRun']) return; // run may have just been called through dependencies being fulfilled just in this very frame

  function doRun() {
    if (Module['calledRun']) return; // run may have just been called while the async setStatus time below was happening
    Module['calledRun'] = true;

    ensureInitRuntime();

    preMain();

    if (Module['_main'] && shouldRunNow) {
      Module['callMain'](args);
    }

    postRun();
  }

  if (Module['setStatus']) {
    Module['setStatus']('Running...');
    setTimeout(function() {
      setTimeout(function() {
        Module['setStatus']('');
      }, 1);
      if (!ABORT) doRun();
    }, 1);
  } else {
    doRun();
  }
}
Module['run'] = Module.run = run;

function exit(status) {
  ABORT = true;
  EXITSTATUS = status;
  STACKTOP = initialStackTop;

  // exit the runtime
  exitRuntime();

  // TODO We should handle this differently based on environment.
  // In the browser, the best we can do is throw an exception
  // to halt execution, but in node we could process.exit and
  // I'd imagine SM shell would have something equivalent.
  // This would let us set a proper exit status (which
  // would be great for checking test exit statuses).
  // https://github.com/kripken/emscripten/issues/1371

  // throw an exception to halt the current execution
  throw new ExitStatus(status);
}
Module['exit'] = Module.exit = exit;

function abort(text) {
  if (text) {
    Module.print(text);
    Module.printErr(text);
  }

  ABORT = true;
  EXITSTATUS = 1;

  throw 'abort() at ' + stackTrace();
}
Module['abort'] = Module.abort = abort;

// {{PRE_RUN_ADDITIONS}}

if (Module['preInit']) {
  if (typeof Module['preInit'] == 'function') Module['preInit'] = [Module['preInit']];
  while (Module['preInit'].length > 0) {
    Module['preInit'].pop()();
  }
}

// shouldRunNow refers to calling main(), not run().
var shouldRunNow = true;
if (Module['noInitialRun']) {
  shouldRunNow = false;
}


run();

// {{POST_RUN_ADDITIONS}}






// {{MODULE_ADDITIONS}}






