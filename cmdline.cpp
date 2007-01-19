/*  Time Based Text - Commandline Player and Recorder
 *
 *  (C) Copyright 2006 - 2007 Denis Rojo <jaromil@dyne.org>
 *
 * This source code is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Public License as published 
 * by the Free Software Foundation; either version 2 of the License,
 * or (at your option) any later version.
 *
 * This source code is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * Please refer to the GNU Public License for more details.
 *
 * You should have received a copy of the GNU Public License along with
 * this source code; if not, write to:
 * Free Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 */


//// system includes
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <inttypes.h>

#include <getopt.h>

#include <sys/types.h>
#include <sys/param.h>
#include <sys/time.h>

#include <assert.h>
#include <signal.h>

//////////////////////


// Time Based Text
#include <tbt.h>



// S-Lang widgets
#include <slw_console.h>
#include <slw_text.h>


// my lovely utils
#include <jutils.h>



// Time Based Text global object
TBT tbt;



///////////////// commandline stuff

void set_status(SLangWidget *s);

static const char *help =
"Usage: tbt [options] [file]\n"
"\n"
"  -h   print this help\n"
"  -v   version information\n"
"  -D   debug verbosity level - default 1\n"
"  -c   console interface mode (S-Lang)\n"
"  -r   record   tbt  - option alias: rectext\n"
"  -p   playback tbt  - option alias: playtext\n"
"  -m   mail composer - option alias: recmail\n"
"  -s   save format in [ bin | ascii | javascript ]\n";

static const char *short_options = "-hvD:crpms:";

int debug;
char filename[512];


// act as commandline tool by default
bool console = false;


// operation modes
#define REC        1
#define PLAY       2
#define MAIL       3
int operation = REC;


// rendering formats
#define BIN        1
#define ASCII      2
#define JS         3
int render = BIN;


uint64_t key;

int c, len;


void cmdline(int argc, char **argv) {
  int res;

  debug = 1;
  filename[0] = 0x0;

  fprintf(stderr, "TBT - Time Based Text - %s\n", VERSION);

  /// adjust the operation to the way the binary is called
  if ( ! strstr(argv[0], "tbt") )
    console = true; // use console by default when binary is aliased
  if      ( strstr(argv[0],"rectext") )
    operation = REC;
  else if ( strstr(argv[0],"playtext") )
    operation = PLAY;
  else if ( strstr(argv[0],"recmail") )
    operation = MAIL;


  do {
    res = getopt(argc, argv, short_options);
    switch(res) {
    case 'h':
      fprintf(stderr, "%s", help);
      exit(0);
      break;

    case 'v':
      fprintf(stderr,"\n");
      exit(0);
      break;

    case 'D':
      debug = atoi(optarg);
      if(debug>3) {
	warning("debug verbosity ranges from 1 to 3\n");
	debug = 3;
      }
      break;

    case 'c':
      console = true;
      break;

    case 'r':
      operation = REC;
      break;

    case 'p':
      operation = PLAY;
      break;

    case 'm':
      operation = MAIL;
      break;

    case 's':
      if( strncasecmp(optarg, "BIN", 3) ==0)
	      render = BIN;
      else if( strncasecmp(optarg, "ASCII", 5) ==0)
	      render = ASCII;
      else if( strncasecmp(optarg, "JAVASCRIPT", 10) ==0)
	      render = JS;
      else {
	      error ("render format not recognized: %s", optarg);
	      act ("using default binary format render");
	      render = BIN;
      }
      break;


    case 1:  snprintf(filename,511,"%s", optarg);
      
    default: break;
      
    }
  } while (res != -1);

  for(;optind<argc;optind++)
    snprintf(filename, 511, "%s", argv[optind]);

  if(!filename[0])
    sprintf(filename, "record.tbt");

  set_debug(debug);

}

///////////// end of commandline stuff




//// console based operations using S-Lang interface

// S-Lang console
SLangConsole *con;

// S-Lang widgets
SLW_Text     *txt;
SLW_Text     *status;

int record_console() {

  // initialize the text console
  if(! con->init() ) return(-1);
  
  //  initialize the status line
  status->border = false;
  status->set_name("status box");
  if(! con->place(status, 0, con->h-10, con->w, con->h) ) {
    error("error placing the status widget");
    return(-1);
  }
  assert ( status->init() );
  
  //  set the status widget *** only after placing it! ***
  set_status(status);
  
  // initialize the text canvas
  txt->set_name("editor");
  // txt->border = true;
  if(! con->place(txt, 0, 0, con->w, con->h-6) ) { //  con->w, con->h-20) ) {
    error("error placing the text widget");
    return(-1);
  }
  assert ( txt->init() );
  
  // focus the text canvas
  con->focused = txt;
  
  
  // write out to the status widget
  notice("TBT - console ready");
  
  while(!tbt.quit) {
    
    key = con->getkey();
    
    if(key) {
      
      // save the key and timestamp
      tbt.append(key);
      
      // display the key
      con->feed(key);
      
    }
    
    if( ! con->refresh() ) tbt.quit = true;
    
    jsleep(0,10);
    
  }

  return 1;
}


int play_console() {
  

  //  initialize the status line
  status->border = false;
  status->set_name("status box");
  if(! con->place(status, 0, con->h-10, con->w, con->h) ) {
    error("error placing the status widget");
    return(-1);
  }
  assert ( status->init() );
  
  //  set the status widget *** only after placing it! ***
  set_status(status);
 
  // place the text canvas
  if(! con->place(txt, 0, 0, con->w, con->h-1) ) {
	  error("error placing the text widget");
	  return(-1);
  }
  txt->set_name("player");
  assert ( txt->init() );
  // focus the text console
  con->focused = txt;



  // main playback loop
  for(c=0; c<len && !tbt.quit; c++) {
    
    // tbt.getkey is a blocking call
    // will wait N time before returning
    key = tbt.getkey();

    con->feed(key);
    if( ! con->refresh() ) tbt.quit = true;

  }
  
  return 1;
}

//////////////////////////////////// end of console based operations


/* signal handling */
void quitproc (int Sig) {
  act("interrupt caught, exiting.");
  tbt.quit = true;
}



int main(int argc, char** argv)
{

  cmdline(argc, argv);
  
  // start the TBT engine
  if(! tbt.init() )  exit(0);

  /* register signal traps */
  if (signal (SIGINT, quitproc) == SIG_ERR) {
    perror ("Couldn't install SIGINT handler"); exit(0); }

  // if playing, load the recording
  if(operation == PLAY) 
    if( ! tbt.load( filename ) ) {
      error("no entries found in file %s",filename);
      exit(1);
    } else
      len = tbt.buffer->len();


  if(console) { // initialize the s-lang console
  
    con = new SLangConsole();
    
    // initialize the text console
    if(! con->init() ) {
      error("cannot initialize S-Lang console on this terminal.");
      exit(0);
    }

    txt    = new SLW_Text();
    status = new SLW_Text();

    switch(operation) {

    case REC:
      record_console();
      break;

    case PLAY:
      play_console();
      break;

    case MAIL:
      error("TODO recmail");
      break;

    }

    // cleanly close the console
    con->refresh();
    jsleep(1,0);
    con->close();


  } else {
    ////////////// commandline operations

    switch(operation){

    case REC:
      // append from 0 (stdin) each 1 byte
      tbt.fdappend(0, 1);
      break;

    case PLAY:

      // main playback loop
      for(c=0; c<len && !tbt.quit; c++) {
	// tbt.getkey is a blocking call
	// will wait N time before returning
	key = tbt.getkey();
	  
	// print out on stdout
	write(1, (void*)&key, 1);
      }
      break;

    }

  }    
  
  notice("Closing Time Based Recorder");

  if(operation == REC) {
    // if we were recording, now save a file

    switch(render) {

    case BIN:
      tbt.save_bin( filename );
      act("TBT file %s rendered in binary format",filename);
      break;
      
    case ASCII:
      tbt.save_ascii( filename );
      act("TBT file %s rendered in ascii format",filename);
      break;

    case JS:
      tbt.save_javascript( filename );
      act("TBT file %s rendered in javascript format",filename);
      break;

    }

    act("%u entries saved.", tbt.buffer->len());
  }

  exit(0);  
}