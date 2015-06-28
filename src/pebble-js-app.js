
var id;

var info = {
  pos: {
  latitude: 0,
  longitude: 0
  },
  addr: {
    postal: '',
    prefecture : '',
    city: '',
    town: ''
  },
  weather: {
    lastCheck: 0,
    rainfall: 0
  }
};

var RAINFALL_CHECK_INTERVAL=10*60*1000;

var yahooAppId = 'dj0zaiZpPXppd29NZklaemx5dyZzPWNvbnN1bWVyc2VjcmV0Jng9ZmY-';

var locationOptions = {
  enableHighAccuracy: true, 
  maximumAge: 0, 
  timeout: 5000
};

var unicodeEscape = function(str) {
    var code, pref = {1: '/u000', 2: '/u00', 3: '/u0', 4: '/u'};
    return str.replace(/[^\x20-\x7f]/g, function(c) {
        return pref[(code = c.charCodeAt(0).toString(16)).length] + code;
    });
};

function executeWeather(pos){
  
  var req2 = new XMLHttpRequest();
  var weatherUrl = 'http://weather.olp.yahooapis.jp/v1/place?appid='+yahooAppId+'&coordinates='+pos.coords.longitude+','+pos.coords.latitude+'&output=json';
  console.log('weather open.. url='+weatherUrl);
  req2.open('GET', weatherUrl, true);
  req2.onload = function(e) {
    if (req2.readyState == 4) {
      console.log("weather status="+req2.status);
      if(req2.status == 200) {
        var weatherArrayReg=/\x22Weather\x22([^\]])*]/;
        var weatherRegRes=weatherArrayReg.exec(unicodeEscape(req2.responseText));
        var json = weatherRegRes!=null ? JSON.parse('{'+weatherRegRes[0]+'}') : '{}';
        if ( json.Weather!=null ){
          var weatherList = json.Weather;
          var maxR = 0.00;
          var wi = weatherList.length;
          for (var i=0 ; i<weatherList.length ; i++ ){
            if ( weatherList[i].Type==='forecast' ){
              var r = weatherList[i].Rainfall;
              if ( maxR<r ){
                maxR = r;
              }
            }
          }
          console.log("weather rainfall="+maxR);
          info.weather.rainfall=maxR;
          Pebble.sendAppMessage({ 'KEY_RAINFALL': parseInt(maxR*100) });
        } else {
          console.log('weather format Error');
        }
      } else { console.log('weather rest Error. http_status='+req2.status); }
    }
  }
  req2.send(null);
  info.weather.lastCheck=Date.now();
  
}

function executeGeoCode(pos){
  
  var req = new XMLHttpRequest();
  var geoapiUrl='http://geoapi.heartrails.com/api/json?method=searchByGeoLocation&x='+pos.coords.longitude+'&y='+pos.coords.latitude;
  console.log('geiapi open.. url='+geoapiUrl);
  req.open('GET', geoapiUrl , true);
  req.onload = function(e) {
    if (req.readyState == 4) {
      if(req.status == 200) {
        console.log(req.responseText);
        var city = 'ERROR';
        var json = JSON.parse(req.responseText);
        if ( !json.response.error && json.response.location ){
          city = json.response.location[0].city+json.response.location[0].town;
          if ( info.addr.city+info.addr.town!=city ){
            info.addr.prefecture = json.response.location[0].prefecture;
            info.addr.city = json.response.location[0].city;
            info.addr.town = json.response.location[0].town;
            info.addr.postal = json.response.location[0].postal;
            Pebble.sendAppMessage({ 'KEY_CITY': city });
            console.log('sendAppMessage city!');
          
            setTimeout(function(){console.log('weatcher setTimeout called.');executeWeather(pos);},0);
            return;
          }
        }
        console.log('city unchanged. skip...');
        
      } else { console.log('geiapi Error'); }
    }
  }
  req.send(null);
  
}

function locationSuccess(pos) {
  
  console.log('Location changed!!');
  console.log('lat= ' + pos.coords.latitude + ' lon= ' + pos.coords.longitude);
  if ( info.pos.latitude==pos.coords.latitude && info.pos.longitude==pos.coords.longitude ){
    console.log('unmoved. skip...');
    if ( info.weather.lastCheck + RAINFALL_CHECK_INTERVAL < Date.now() ){
      setTimeout(function(){console.log('weatcher setTimeout called.');executeWeather(pos);},0);
    }
    return;
  }
  info.pos=pos.coords;
  setTimeout(function(){console.log('setTimeout called.'); executeGeoCode(pos);},0);
}

function locationError(err) {
  console.log('location error (' + err.code + '): ' + err.message);
}

function executeGeoLocation(){
      navigator.geolocation.getCurrentPosition (locationSuccess, locationError, locationOptions);
}

Pebble.addEventListener('ready',
  function(e) {
    executeGeoLocation();
    setInterval(function(){
      executeGeoLocation();
    },180*1000);
    console.log('JavaScript app ready and running!');
  }
);