#       GURADRIAN TALES DISCORD BOT
>       UNDER DEVELOPMENT ONLY FOR LINUX

TODO LIST:

- [x] add discord SlashCommands :smirk:
- [ ] add cron function for automatic coupon checker :fearful:
- [ ] add auto Coupon Redeemer  :bowtie:
- [ ] add arena stuff :satisfied:
- [ ] fix and clean the repo :flushed:



TO BUILD:
```
git clone https://github.com/Alaory/guardian-tales-discord-bot
cd guardian-tales-discord-bot
mkdir build && cd build
cmake .. -DTOKEN="\"your discord bot token\"\"
```
also you need a firebase google-services.json file and drop it in the build folder
and change the link from the database.hpp file to your links
dont forget to add the files to storage on the firbase 

thanks to:

    - firebase
    - DPP
    - httplib
    - myhtml
    - nlohmann/json
    - Scheduler

for the librays
