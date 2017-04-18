#include <Thread.h> // Include the Thread library

#define TANK_FULL_PIN   2   // Float Switch plugged into Digital Pin 2
#define DRAIN_PUMP_PIN  6   // Drain Pump plugged into Digital Pin 6
#define FEED_PUMP_PIN   7   // Feeder pump plugged into Digital Pin 7

const int cooldownMilis = 10000; // cooldown delay 10 seconds.

// Variables used to track the state of the system.
bool tankFullState = false;  // is the Water storage tank full?

Thread checkSensorsThread = Thread();
Thread updatePumpsThread = Thread();
Thread drainCooldownThread = Thread();


// This method is the callback for the checkSensorsThread
// It should set all of the STATE variables for each of the
// sensors in the system.
void doCheckSensors()
{
  tankFullState = digitalRead(TANK_FULL_PIN) == LOW;  // the right of the = operator
                                                      // is a boolean statement checking
                                                      // wheter the float switch is 
                                                      // closed or open.

  // When new sensors are added to the system, you should add the associated
  // sensor reading code to this.
  // This is not an ACTION thread.  It should -only- update STATE varuables.
}


// This is the callback for the updatePumps thread.
// This method should take the STATE variables and set
// the actions for the pumps accordingly.
void doUpdatePumps()
{
  // if the tank is full AND I'm NOT already cooling down.
  if(tankFullState && !drainCooldownThread.enabled)
  {
    digitalWrite(DRAIN_PUMP_PIN, HIGH); // Turn OFF the drain pump.
    drainCooldownThread.enabled = true;             // enable the cooldown.
    drainCooldownThread.setInterval(cooldownMilis); // the constant cooldown defined above.
    Serial.println("Tank is full!");
  }

  // if the tank is not full BUT I'm still cooling down.
  if(!tankFullState && drainCooldownThread.enabled)
  {
    Serial.println("I'm still cooling down....");
    // don't do anything....
  }
  // tank is full and I'm NOT cooling down (this is caught in the -else-)
  else if(!tankFullState)
  {
    digitalWrite(DRAIN_PUMP_PIN, LOW) // Turn ON the drain pump.
    drainCooldownThread.enabled = false;     // disable the cooldown.
    drainCooldownThread.setInterval(320000); // really long time...
  }
}


// Run this when the drain cooldown is done.
void drainCooldownComplete()
{
  drainCooldownThread.enabled = false;     // cooldown complete.  Stop.
  drainCooldownThread.setInterval(320000); // really long time...
  Serial.println("Cooldown complete!");
}

void setup() 
{
  // set the callback function for the drainCooldownThread.
  drainCooldownThread.onRun(drainCooldownComplete);

  // set the callback function for the updatePumpsThread.
  updatePumpsThread.onRun(doUpdatePumps);
  updatePumpsThread.setInterval(100); // run at 10Hz

  // set the callback function for the checkSensorsThread.
  checkSensorsThread.onRun(checkSensors);
  checkSensorsThread.setInterval(100); // run at 10Hz  
}

void loop() 
{
  // Check if each thread needs to run, and if so, run it.
  if(checkSensorsThread.shouldRun())  checkSensorsThread.run();
  if(updatePumpsThread.shouldRun())   updatePumpsThread.run();
  if(drainCooldownThread.shouldRun()) drainCooldownThread.run();
}
