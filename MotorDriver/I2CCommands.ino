#include <Wire.h>

// List of the supported I2C commands
enum {
  I2C_CMD_STOP = 0x11,
  I2C_CMD_SET_MOTORS = 0x12

};

// Max values for the I2C buffers
enum {
  I2C_MSG_ARGS_MAX = 32,
  I2C_RESP_LEN_MAX = 32
};

/* Typedef of an i2c callback function
 *
 * @param i2cArgs Any arguments passed on i2c
 * @param pi2cResponse Filled in with the I2C response, if any
 *
 * @returns the number of items added to the response
 */
typedef int (*i2cCallback)(byte *i2cArgs, uint8_t *pi2cResponse);

// Structure for holding details on a single i2c command
typedef struct i2cCommand {
  byte command;
  byte numberOfArgs;
  i2cCallback fnCallback;
};


// Lookup table of all the supported commands.
// Detailing the command number, number of arguments
// and the function to call to proess it
extern const i2cCommand supportedI2Ccmd[] = {
  { I2C_CMD_STOP, 0, motorsI2CStop},
  { I2C_CMD_SET_MOTORS, 4, motorsI2CSet}
};

// The i2c address we will be using
#define SLAVE_ADDRESS 0x07

static int i2cMaxCommands = (sizeof(supportedI2Ccmd) / sizeof(supportedI2Ccmd[0])); //<! Number of i2c commands in the array

int argsCnt = 0;                        //<! how many arguments were passed with given command

byte i2cArgs[I2C_MSG_ARGS_MAX];         //<! array to store args received from master
int i2cArgsLen = 0;                     //<! how many args passed by master to given command

uint8_t i2cResponse[I2C_RESP_LEN_MAX];  //<! array to store response
int i2cResponseLen = 0;                 //<! response length

const i2cCommand *requestedCmd = NULL; //<! The currently proessing command

/**
 * Configures the i2c communication ready
 * for use
 */
void I2CSetup() {
  // initialize i2c as slave
  Wire.begin(SLAVE_ADDRESS);

  // define callbacks for i2c communication
  Wire.onReceive(receiveData);
  Wire.onRequest(sendData);
}

/**
 * Checks if there is a pending i2c command to process.
 */
void I2C_CheckCommands() {
  if(requestedCmd) {
    int extraArgs = 0;

    // The first argument is always the command number
    // so the callee can confirm its been executed
    i2cResponseLen = 0;
    i2cResponse[i2cResponseLen++] = requestedCmd->command;

    // Trigger the callback function
    extraArgs = requestedCmd->fnCallback(i2cArgs, &i2cResponse[1]);
    i2cResponseLen += extraArgs;

    // Clear pointer so we don't trigger it twice
    requestedCmd = NULL;
  }
}


/**
 * callback for received I2C data
 *
 * @param howMany Number of bytes that arrived
 */
void receiveData(int howMany) {
  int cmdRcvd = -1;
  int argIndex = -1;
  int availableBytes = 0;
  argsCnt = 0;

  if(requestedCmd) {
    Serial.println("Command lost!");
  }

  if (howMany) {
    // receive first byte - command assumed
    cmdRcvd = Wire.read();

    // Skip the 'command' byte
    howMany--;

    // Anything left to process?
    if(howMany) {
      while(howMany--){
        // receive rest of transmission from master assuming arguments to the command
        if (argIndex < I2C_MSG_ARGS_MAX){
          argIndex++;
          i2cArgs[argIndex] = Wire.read();
        }
        else{
          ; // implement logging error: "too many arguments"
        }
        argsCnt = argIndex+1;
      }
    }
  }
  else{
    // TODO: implement logging error: "empty request"
    return;
  }

  // validating command is supported by slave
  int fcnt = -1;
  for (int i = 0; i < i2cMaxCommands; i++) {
    if (supportedI2Ccmd[i].command == cmdRcvd &&
        supportedI2Ccmd[i].numberOfArgs == argsCnt)
    {
      fcnt = i;
    }
  }

  if (fcnt<0){
    Serial.print("Command not supported");
    Serial.print(cmdRcvd);
    Serial.print(":");
    Serial.println(argsCnt);
    return;
  }


  // Note the selected command
  requestedCmd = &supportedI2Ccmd[fcnt];

  // now main loop code should pick up a command to execute and prepare required response when master waits before requesting response
}

// callback for sending data
void sendData(){
  // Send the cached response
  Wire.write(i2cResponse, i2cResponseLen);

  // and clear the length so we don't send it again
  i2cResponseLen = 0;
}

