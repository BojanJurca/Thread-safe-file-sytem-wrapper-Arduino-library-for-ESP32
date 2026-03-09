// 1️⃣ Include a file system of your choice
#include <LittleFS.h>             // Or SPIFFS.h or FFat.h or SD.h ...


// 2️⃣ Include thread-safe wrapper
#include <threadSafeFS.h>         // Include thread-safe wrapper since LittleFS, FFat and SD file systems are not thread safe


// 3️⃣ Crete thread-safe wrapper instance arround LittleFS (or SPIFFS or FFat or SD ...)
threadSafeFS::FS TSFS (LittleFS);


// 4️⃣ Simplify programming by changing the meaning of File from fs::File to threadSafeFS::File
using File = threadSafeFS::File;  // Use thread-safe wrapper for all file operations form now on in your code


void setup () {
  Serial.begin (115200);


  // 5️⃣ Start LittleFS (or SPIFFS or FFat or SD ...)
  LittleFS.begin (true);


  // 6️⃣ Use thread-safe wrapper instance from now on


  // Create test file
  if (!TSFS.exists ("/test.txt")) {
    File f = TSFS.open ("/test.txt", "w");
    if (f) {
      f.print ("This is a test file.");
      f.close ();
      Serial.printf ("/test.txt created\n");
    } else {
      Serial.printf ("/test.txt could not be created\n");
    }
  }

  // Read it periodically in two separate tasks, one of them is setup - loop, the other one is lambda function below
  xTaskCreate ([] (void *param) {
                                  while (true) {
                                    delay (900);

                                    File f = TSFS.open ("/test.txt", "r");
                                    if (f) {

                                      // read file content
                                      char buf [100];
                                      int i = f.read ((uint8_t *) buf, sizeof (buf) - 1);
                                      buf [i] = 0;
                                      f.close ();

                                      Serial.printf ("                                    %s\n", buf);

                                    } else {
                                      Serial.printf ("                                    /test.txt could not be read\n");
                                    }

                                  }
                                  vTaskDelete (NULL);
                                }, 
                                "the_other_task", 4068, NULL, 1, NULL);

}

void loop () {
  delay (1000);

  File f = TSFS.open ("/test.txt", "r");
  if (f) {

    // read file content
    char buf [100];
    int i = 0;
    while (i < sizeof (buf) - 1 && f.available ())
      buf [i ++] = f.read ();
    buf [i] = 0;

    f.close ();

    Serial.printf ("%s\n", buf);

  } else {
    Serial.printf ("/test.txt could not be read\n");
  }

}
