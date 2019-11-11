#include <iostream>
#include <fstream>
#include "errors.h"
#include "enigma.h"
using namespace std;

/* -------- Declare functions -------- */
int error_finder(int errorcode, char const *filename = nullptr);
int config_checker(char const *filename, int inputcounter, int totalcount);
int component_config_checker(char const *filename, int totalcount);
int rotorpos_config_checker(char const  *filename, int totalcount);

/* -------- Main function -------- */
int main(int argc, char** argv) {
  int errorcode = 0;
  // Check for INSUFFICIENT_NUMBER_OF_PARAMETERS error
  if (argc<4) {return error_finder(INSUFFICIENT_NUMBER_OF_PARAMETERS);}
  // Check all inputs for configuration errors
  for (int count = 1; count<argc; count++) {
    errorcode = config_checker(argv[count],count,argc);
    if (errorcode!=0) {return errorcode;}
  }
  // Make the EnigmaMachine:
  EnigmaMachine Enigmamachine = EnigmaMachine(argv[1], argv[2]);
    
  if (argc>4){
    ifstream in;
    int notch;
    int count = 3;
    in.open(argv[argc-1]);
    while (!(in>>notch).fail()) {
      Enigmamachine.add_rotor(argv[count],notch);
      count++;
    }
    in.close();
  }
  
  // Take Input and encrypt, returning an INVALID_INPUT_CHARACTER error if necessary
  char input;
  while (!(cin>>input).fail()) {
    if (input<65||input>95) {Enigmamachine.enigma_delete(); return error_finder(INVALID_INPUT_CHARACTER);}
    cout << Enigmamachine.enigma_encrypt(input);
  }
  Enigmamachine.enigma_delete();
  cout << endl;
  return error_finder(NO_ERROR);
}
/* -------- Define functions -------- */
// Error Finder Function 
int error_finder(int errorcode, char const *filename) {
  switch(errorcode) {
  case 0 : return errorcode;
  case 1 : cerr << "Insufficient number of input parameters provided." << endl; return errorcode;
  case 2 : cerr << endl << "Invalid input character provided. " << endl; return errorcode;
  case 3 : cerr << "Invalid index provided during configuration in file " << filename << endl; return errorcode;
  case 4 : cerr << "Non-numeric character in plugboard file " << filename << endl; return errorcode;
  case 5 : cerr << "Impossible plugboard configuration provided in file " << filename << endl; return errorcode;
  case 6 : cerr << "Incorrect number of characters in plugboard file " << filename << endl; return errorcode;
  case 7 : cerr << "Invalid rotor mapping during configuration in file: " << filename << endl; return errorcode;
  case 8 : cerr << "No rotor starting position provided in file: " << filename << endl; return errorcode;
  case 9 : cerr << "Invalid reflector mapping during configuration in file: " << filename << endl; return errorcode;
  case 10 : cerr << "Incorrect number of reflector parameters during configuration in file: " << filename << endl; return errorcode;
  case 11 : cerr << "Unable to open configuration file: " << filename << endl; return errorcode;
  default : cerr << "Unknown error code submitted!" << errorcode << endl; return errorcode;;  
  }
}
// Configuration Checker Function
int config_checker(char const *filename, int inputcounter, int totalcount) {
  if (inputcounter < totalcount-1) {
    return component_config_checker(filename,inputcounter);
  } else {
    return rotorpos_config_checker(filename, totalcount);
  }
}

// Plugboard/Reflector Configuraton Checker
// Called by Configuraton Checker Function
int component_config_checker(char const *filename, int inputnumber) {
  // Declare (and) Initialise Variables
  ifstream in;
  int integer;
  int temparray[26] = {0};
  int counter = 0;
  // Check for ERROR_OPENING_CONFIGURATION_FILE error:
  in.open(filename);
  if (in.fail()) return error_finder(ERROR_OPENING_CONFIGURATION_FILE,filename);
  // Run through the file contents and check for errors:
  while (!(in>>integer).eof()) {
    // Check for NON_NUMERIC_CHARACTER error:
    if (in.fail()) {in.close(); return error_finder(NON_NUMERIC_CHARACTER,filename);}
    // Check for INVALID_INDEX error:
    if ((integer<0)||(integer>25)) {in.close(); return error_finder(INVALID_INDEX,filename);}
    // Check for IMPOSSIBLE_PLUGBOARD_CONFIGURATION or INVALID_REFLECTOR_MAPPING or INVALID_ROTOR_MAPPING error:
    if ((counter<26) && temparray[integer]>0) {
      in.close();
      switch(inputnumber) {
      case 1 : return error_finder(IMPOSSIBLE_PLUGBOARD_CONFIGURATION,filename);
      case 2 : return error_finder(INVALID_REFLECTOR_MAPPING,filename);
      default : return error_finder(INVALID_ROTOR_MAPPING,filename);
      }
    }
    temparray[integer]++;
    counter++;
  }
  // Check for INCORRECT_NUMBER_OF_PLUGBOARD_PARAMETERS / INCORRECT_NUMBER_OF_REFLECTOR PARAMETERS error:
  switch(inputnumber) {
  case 1 : if (counter%2!=0) {in.close(); return error_finder(INCORRECT_NUMBER_OF_PLUGBOARD_PARAMETERS,filename);} break;
  case 2 : if (counter!=26) {in.close();cout<< inputnumber <<endl; return error_finder(INCORRECT_NUMBER_OF_REFLECTOR_PARAMETERS,filename);} break;
  default : if (counter!=27) {in.close(); return error_finder(INVALID_ROTOR_MAPPING,filename);} break;
  }
  // Close file and return NO_ERROR
  in.close();
  return error_finder(NO_ERROR);
}

// Rotor Position Configuraton Checker
// Called by Configuraton Checker Function
int rotorpos_config_checker(char const *filename, int totalcount) {
  // Declare variables
  int rot_count = 0;
  int integer;
  ifstream in;
  // Check that file can be opened
  in.open(filename);
  if (in.fail()) {return error_finder(ERROR_OPENING_CONFIGURATION_FILE,filename);}
  // While loop to iterate contents of file
  while (!(in>>integer).eof()) {
    // Check that the contents are numeric
    if (in.fail()) {in.close(); return error_finder(NON_NUMERIC_CHARACTER,filename);}
    // Check that the contents are a valid index
    if ((integer<0)||(integer>25)) {in.close(); return error_finder(INVALID_INDEX,filename);}
    // Update counter
    rot_count++;
  }
  // Close stream and return count
  in.close();
  // Check rotor position file
  if (rot_count<(totalcount-4)) {return error_finder(NO_ROTOR_STARTING_POSITION,filename);}
  //if (rot_count!=(totalcount-4)) {return error_finder(INSUFFICIENT_NUMBER_OF_PARAMETERS);}
  return error_finder(NO_ERROR);
}

