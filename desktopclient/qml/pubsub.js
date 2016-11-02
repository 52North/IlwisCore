// adapted from http://stackoverflow.com/questions/28507619/how-to-create-delay-function-in-qml
/*function Timer() {
    return Qt.createQmlObject("import QtQuick 2.0; Timer {}", root);
}
*/

/*var timer = new Timer();
timer.interval = 0;
timer.repeat = false;
timer.triggered.connect(function () {
    print("I'm triggered once every second");
})
*/
//timer.start();

/*
function setTimeout(func, timeout) {
    var timer = new Timer();
    timer.interval = 0;
    timer.repeat = false;
    timer.triggered.connect(func);
    timer.start();
}
*/

// adapted from https://msdn.microsoft.com/en-us/magazine/hh201955.aspx


var PubSub = {};

(function(p){

//    "use strict"; // might not be needed


    var topics = {},
          lastUid = -1;


    var publish = function( topic /*, token*/ , data){
        //timer.stop();

        if ( !topics.hasOwnProperty( topic ) ){
            return false;
        }

        var notify = function(){

            var subscribers = topics[topic],
                throwException = function(e){

                return function(){
                    throw e;
                };
            };


            for ( var i = 0, j = subscribers.length; i < j; i++ ){
                try {
                    //if (subscribers[i].token != token)
                        subscribers[i].func( topic, /*token,*/ data );
                } catch( e ){
                    //setTimeout( throwException(e), 0);
                    throwException(e);
                }
            }
        };

        //setTimeout( notify , 0 );

        //timer.triggered.connect(notify)
        //timer.start();

        notify();

        return true;
    };



    /**
     *  Publishes the topic, passing the data to it's subscribers
     *  @topic (String): The topic to publish
     *  @data: The data to pass to subscribers
    **/

    p.publish = function( topic, /*token,*/ data ){
        return publish( topic, /*token,*/ data, false );
    };



    /**
     *  Subscribes the passed function to the passed topic.
     *  Every returned token is unique and should be stored if you need to unsubscribe
     *  @topic (String): The topic to subscribe to
     *  @func (Function): The function to call when a new topic is published
    **/

    p.subscribe = function( topic, func ){

        // topic is not registered yet

        if ( !topics.hasOwnProperty( topic ) ){
            topics[topic] = [];
        }

        var token = (++lastUid).toString();

        topics[topic].push( { token : token, func : func } );

        // return token for unsubscribing
        return token;
    };

    /**

     *  Unsubscribes a specific subscriber from a specific topic using the unique token
     *  @token (String): The token of the function to unsubscribe

    **/

    p.unsubscribe = function( token ){

        for ( var m in topics ){
            if ( topics.hasOwnProperty( m ) ){
                for ( var i = 0, j = topics[m].length; i < j; i++ ){
                    if ( topics[m][i].token === token ){
                        topics[m].splice( i, 1 );
                        return token;
                    }
                }
            }
        }
        return false;
    };
}(PubSub));



function subscribe (topic, func) {
    return PubSub.subscribe( topic, func );
}

function publish(topic, /*token,*/ data) {
    return PubSub.publish(topic, /*token,*/  data);
}

function unsubscribe(token) {
    //setTimeout(function(){
        PubSub.unsubscribe( token );
    //}, 0);
}



