var io = require('socket.io')(3000);
var playerCounter = 0;
var roomID = "";
var playerData = [];
var waitingPlayers = [];

io.on('connection', function (socket) {
        console.log("new connection: ", socket.id);

        //send the api key to connected client.
        socket.emit('setkey', {'ak' : "9a08a263-25a2-4d97-b2a6-fe4eb6021fcb"});

        //when client calls matchmake then we do this stuff.
        /* this data is sent to server as part of matchmake.
           {
           "id": 19248048,
           "name": "GiZmOForEver",
           "profileIconId": 715,
           "revisionDate": 1446037652000,
           "summonerLevel": 30
           "cards" : [] //list of cards
           }
        */    
        socket.on('matchmake', function(data){
            playerCounter++;
            waitingPlayers.push(socket.id);
            playerData.push(JSON.parse(data));
            
            if(playerCounter == 1){
                //pick the room id of the first player.
                roomID = "room_" + socket.id;

                console.log("RoomID: ", roomID);
                socket.join(roomID);

            }else if(playerCounter == 2){
                var data2Send = playerData;
                //the second player joins the same room as the first one.
                socket.join(roomID, function(err){
                    console.log("Broadcasting..", err);
                    var yourArray = [0, 1, 2, 3, 4, 5, 6, 7, 8, 9];
                    yourArray.sort(function() { return 0.5 - Math.random() });
                    //broadcast that match is ready.
                    matchobj = {
                        "p" : data2Send,
                        "s" : yourArray
                    };
                    
                    console.log("Broadcasting to : " + roomID);
                    socket.to(roomID).emit('matchmade', matchobj);
                    
                    //send to the last guy joining. (this is because the guy who just joined wasnt' receiving this event).
                    socket.emit('matchmade', matchobj);
                    });
                
                //reset for the next set of players.
                playerData = [];
                playerCounter = 0;
                waitingPlayers = [];
            }
            socket.gameroom = roomID;
        });

        //when clinet sends an event do something on server.
        socket.on('move', function (data) {
            console.log("GotMove: " + data);
            socket.broadcast.to(socket.gameroom).emit("move", JSON.parse(data));
        });

        socket.on('gameover', function(data) {
            console.log("Game Done");
            socket.leave(socket.gameroom, function(err){
                console.log("Left room: " + err);
            });
        });

        socket.on('disconnect', function(){
            console.log("Disconnected: ", socket.id);
            
            //see if this connection was in waiting list.
            var index = waitingPlayers.indexOf(socket.id);
            if(index < 0){
                console.log("Informing the other players");
                socket.broadcast.to(socket.gameroom).emit("dc", "asdf");
            }else{                
                console.log("Removed from Waiting list");
                //remove the player from waiting list.
                waitingPlayers.splice(index, 1);
                playerData.splice(index, 1);
                playerCounter--;
            }
        });
});
