#       GURADRIAN TALES DISCORD BOT
>       UNDER DEVELOPMENT ONLY FOR LINUX


[![Follow me](https://img.shields.io/github/followers/Alaory?style=for-the-badge)](https://github.com/Alaory)

TODO LIST:

- [x] add discord SlashCommands :smirk:
- [x] add cron function for automatic coupon checker :fearful:
- [x] add slashcommand for user id and name
- [ ] <del> add auto Coupon Redeemer </del> :bowtie: //too slow need to find other way to do it
- [ ] add arena stuff :satisfied:
- [ ] fix and clean the repo :flushed:
- [ ] <strong>OPTIONAL</strong> add news and best charater selector :smile:


TO BUILD:
```
git clone https://github.com/Alaory/guardian-tales-discord-bot
cd guardian-tales-discord-bot
mkdir build && cd build
cmake .. -DTOKEN=your discord bot token
make
```
<strong>IMPORTANT</strong><br>
you need a firebase google-services.json file. drop it in the build folder
and change the link from the database.hpp file to your files links and
<strong>dont forget to add the files to storage on the firebase</strong>

if you dont want to use the firebase stuff tell me and i'll make a branch with out the firebase and replace it with local files

thanks to:

    - firebase
    - DPP
    - httplib
    - myhtml
    - nlohmann/json
    - Scheduler

for the librays
