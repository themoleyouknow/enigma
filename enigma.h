#include <iostream>
#include "errors.h"
using namespace std;

// Declare global constant to represent number of letter in alphabet:
int const ALPHABET = 26;
// Forward declaration of subclasses:
class Plugboard;
class Reflector;
class Rotor;

/* ----- Define Enigma Class ---- */
class Enigma {
  friend class EnigmaMachine;
protected:
  //Define Variables
  int notch = -1; // Initialise to -1, overwritten to a non-negative number if a rotor
  Enigma *next_ptr = nullptr;
  Enigma *prev_ptr = nullptr;
  int signalboard[ALPHABET] = {0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25};

  // Class Function Definitions
  
  /* -- enigma_read -- */
  // Function that takes as input a filename, and reads the file in order to
  // set up the signalboard of a plugboard/reflector.
  // NB A rotor has its own read function!
  void enigma_read(char *filename) {
    // Declare variables
    ifstream in;
    int integer1, integer2;
    // Open file
    in.open(filename);
    // Run through file contents
    while (!(in>>integer1).fail() && !(in>>integer2).fail()) {
      // Make the switches
      this->signalboard[integer1]=signalboard[integer1]^signalboard[integer2];
      this->signalboard[integer2]=signalboard[integer2]^signalboard[integer1];
      this->signalboard[integer1]=signalboard[integer1]^signalboard[integer2];
    }
    // Close file
    in.close();
  }
  
  /* -- set_next_ptr -- */
  // Function to set this object's next_ptr:
  void set_next_ptr(Enigma *next_ptr) {
    this->next_ptr = next_ptr;
  }
  
  /* -- set_prev_ptr -- */
  // Function to set this object's prev_ptr:
  void set_prev_ptr(Enigma *prev_ptr) {
    this->prev_ptr = prev_ptr;
  }
  
  /* -- encrypt -- */
  // Function that takes as input an integer and encrypts it using this object's signalboard
  int encrypt(int integer) {
    return integer = this->signalboard[integer];
  }
  
  /* -- rotate -- */
  // Function that takes as input an integer to represent the number of times the object
  // is rotated, initialised to 1 if none provided, and then shifts the signalboard by -1.
  void rotate(int rot_num = 1, bool initial_config = false) {
    // Declare iterations variable
    int iterations = rot_num;
    // While loop to to perform correct number of rotations
    while (iterations>=1) {
      // For loop to loop through signalboard, swapping each item with the next (shift -1): 
      for (int count = 0; count<(ALPHABET-1); count++) {
        this->signalboard[count]=this->signalboard[count]^this->signalboard[count+1];
        this->signalboard[count+1]=this->signalboard[count+1]^this->signalboard[count];
        this->signalboard[count]=this->signalboard[count]^this->signalboard[count+1];
      }
      // Decrease iterations
      iterations--;
    }
    // Check to see if we've reached the notch on this rotor, the next item along is a rotor,
    // and we aren't in the initial configuration stage; if so, rotate next rotor:
    if (this->signalboard[0]==this->notch && this->next_ptr->notch>=0 /*&& !initial_config*/) {
      this->next_ptr->rotate();
    }
  }
  
  /* -- swap_ptr -- */
  // Function that swaps the prev_ptr and next_ptr attributes of this object, as well as
  // reconfigures the object's signalboard to for a reverse signal!
  void swap_ptrs() {
    // Declare temporary swap_ptr and initialise to next_ptr
    Enigma* swap_ptr = this->next_ptr;
    // Declare inverse copy of signalboard
    int sigcopy[ALPHABET] = {0};
    // Perform swap
    this->next_ptr = this->prev_ptr;
    this->prev_ptr = swap_ptr;
    // Nest for loop to determine the inverse signalboard:
    for (int count1 = 0; count1<ALPHABET;count1++) {
      for (int count2 = 0; count2<ALPHABET;count2++) {
	if (count1  == this->signalboard[count2]) {sigcopy[count1]=count2;break;}
      }
    }
    //  For loop to assign all values in current signalboard to inverse signalboard:
    for (int count = 0; count<ALPHABET;count++) {
      this->signalboard[count] = sigcopy[count];
    }
  }
  
  /* -- Printers -- */
  void print_enigma() {
    cout << "               A B C D E F G H I J K L M N O P Q R S T U V W X Y Z " << endl;
    cout << "The board is: [";
    for (int count = 0; count<ALPHABET; count++) {
      cout << char (this->signalboard[count]+65) << " ";
    }
    cout << "]" << endl;
    cout << "               A B C D E F G H I J K L M N O P Q R S T U V W X Y Z " << endl;
  }
};

/* -----  Define Plugboard Class    ----- */
class Plugboard : public Enigma {
  // Declare Variables
  friend class EnigmaMachine;
  /* -- Constructors -- */  
  Plugboard(char *filename) {
    enigma_read(filename);
  }
};

/* -----  Define Reflector Class    ----- */
class Reflector : public Enigma {
  friend class EnigmaMachine;
  /* -- Constructors -- */  
  Reflector(char *filename) {
    enigma_read(filename);
  }
};

/* -----  Define Rotor Class    ----- */
class Rotor : public Enigma {
  friend class EnigmaMachine;
  /* -- Rotor Constructor -- */
  Rotor(char* filename, Enigma *prev_ptr, Enigma *next_ptr, int initial_pos) {
    this->rotor_read(filename);
    this->prev_ptr = prev_ptr;
    this->next_ptr = next_ptr;
    this->rotate(initial_pos,true);
  }
  /* --  Rotor Read -- */
  void rotor_read(char* filename) {
    // Declare variables
    ifstream in;
    int integer, count = 0;
    // Open file
    in.open(filename);
    // Run through file contents
    while (!(in>>integer).fail()) {
      // Check if we're at the final index, if so then update notch:
      if (count==ALPHABET) {notch=this->signalboard[integer];}
      // Set the rotor:
      this->signalboard[count]=integer;
      count++;
    }
    // Close file
    in.close();
  }
};

/* ---- Define EnigmaMachine Class ---- */
class EnigmaMachine {
  Enigma *plugboard_ptr = nullptr;
  Enigma *reflector_ptr = nullptr;
  Enigma *next_ptr = nullptr;
public:
  /* -- Constructor -- */
  EnigmaMachine(char *plug_filename, char *ref_filename) {
    // Construct plugboard and reflector on heap:
    Plugboard *plugboard = new Plugboard(plug_filename);
    Reflector *reflector = new Reflector(ref_filename);
    // Set next_ptr of each to the other:
    plugboard->set_next_ptr(reflector);
    reflector->set_next_ptr(plugboard);
    // Set EnigmaMachine internal pointers:
    this->plugboard_ptr = plugboard;
    this->reflector_ptr = reflector;
    // Set EngigmaMachine's next_ptr to the reflector:
    this->next_ptr = reflector;
  }
  /* -- add_rotor -- */
  // Function that takes as input a rotor configuration filename, and an initial rotor position.
  // This function adds a rotor to the Enigmamachine by constructing a rotor as per the
  // configuration file settings, and adds it to the existing circular linked list.
  void add_rotor(char *rotor_filename,int initial_pos) {
    // Construct new rotor, with prev_ptr set to current prev_ptr, and next_ptr set to Plugboard
    Rotor *rotor = new Rotor(rotor_filename,this->plugboard_ptr,this->next_ptr, initial_pos);
    // Set Plugboard next_ptr to new rotor
    this->plugboard_ptr->set_next_ptr(rotor);
    // If next is reflector, set next_ptr to rotor, else set next_ptr's prev_ptr to new rotor
    if (this->next_ptr==this->reflector_ptr) {
      this->next_ptr->set_next_ptr(rotor);
    } else {this->next_ptr->set_prev_ptr(rotor);}
    // Set prev_ptr to new rotor
    this->next_ptr = rotor;
  }

  /* -- enigma_encrypt -- */
  // Function that takes as input a character and runs through the linked list
  // of existing machine components (at least a plugboard and reflector) to
  // return an encrypted character
  char enigma_encrypt(char character) {
    // Shift ASCII input (capital letter) to equivalent integer value: 
    character =  character-65;
    // Declare pointer variable:
    Enigma *next = this->plugboard_ptr->next_ptr;
    // Use plugboard to encrypt input character:
    character = this->plugboard_ptr->encrypt(character);
    // Rotate first rotor by 1 (default of rotate()), if there is one:
    if ((next->notch)>=0) {next->rotate();}
    // While loop to process character input through the enigma machine:
    while (next!=this->plugboard_ptr) {
      // Encrypt the character using the current object's signalboard:
      character = next->encrypt(character);
      // Check if current object is a rotor:
      if (next->notch>=0) {
	// If a rotor, swap the object's pointers to anticipate a return signal/reset:
	next->swap_ptrs();
	// Set next pointer to this object's prev_ptr (forward direction after swapping!):
	next=next->prev_ptr;
	// If not a rotor, set next pointer to this object's next_ptr:
      }else {next=next->next_ptr;}
    }
    // Use plugboard to encrypt return signal:
    character = this->plugboard_ptr->encrypt(character);
    // Re-shift character to correct position in ASCII table and return:
    return character + 65;
  }
  
  /* -- enigma_delete -- */
  // Function to delete an enigma machine's components from the heap!
  void enigma_delete() {
    // Declare pointer variables:
    Enigma *next = this->plugboard_ptr->next_ptr; // initialised to point to object after plugboard
    Enigma *current = nullptr;
    // While loop to check that we haven't reached the reflector:
    while (next!=this->reflector_ptr) {
      // Set next pointer to this object's next_ptr
      next  = next->next_ptr;
      // Set current to next pointer
      current = next;
      // Delete the current object
      delete current;
    }
    // Delete reflector and plugboard:
    delete this->reflector_ptr;
    delete this->plugboard_ptr;
  }
  
  void traverse() {
    Enigma *next;
    Enigma *current;
    next = this->plugboard_ptr->next_ptr;
    this->plugboard_ptr->print_enigma();
    current = next;
    while (next!=this->plugboard_ptr){
      next->print_enigma();
      next = next->next_ptr;
      if (current->notch>=0) {current->swap_ptrs();}
      current = next;
    }
    this->plugboard_ptr->print_enigma();
  }
  
};
