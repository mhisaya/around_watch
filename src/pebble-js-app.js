
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
    rainfall: 0,
    condition: ''
  },
  lastCheck: {
    geocode: 0,
    rainfall: 0,
    weather: 0
  }
};

var RAINFALL_CHECK_INTERVAL=10*60*1000;
var WEATHER_CHECK_INTERVAL=60*60*1000;

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
  
  var curDate = new Date();
  var dateStr=[
    1900+curDate.getYear(),
    ('0'+(curDate.getMonth()+1)).replace(/0([1-2][0-9])/,'$1'),
    ('0'+curDate.getDate()).replace(/0([1-3][0-9])/,'$1')
  ].join('-');
    
  var curDateTimeStr = dateStr+' '+('0'+(curDate.getHours()-(curDate.getHours()%3))).replace(/0([1-2][0-9])/,'$1')+':00:00';
  console.log('basetime='+curDateTimeStr);
                                            
  var req2 = new XMLHttpRequest();
  var weatherUrl = 'http://api.openweathermap.org/data/2.5/forecast?units=metric&lon='+pos.coords.longitude+'&lat='+pos.coords.latitude;
  console.log('weather open.. url='+weatherUrl);
  req2.open('GET', weatherUrl, true);
  req2.onload = function(e) {
    if (req2.readyState == 4) {
      console.log("weather status="+req2.status);
      if(req2.status == 200) {
        var json = JSON.parse(req2.responseText);
        if ( json.list!=null ){
          var dispData = [];
          for ( var i=0 ; i<json.list.length && dispData.length<3 ; i++ ){
            var data = json.list[i];
            if ( curDateTimeStr < data.dt_txt ){
              // hour[2]+icon[3]+id[3]+temp[3]
              var temp;
              if ( data.main.temp>=0 ){
                temp='0'+(Math.floor(data.main.temp));
                 temp='0'+temp.substr(temp.length-2,2);
              } else {
                temp='0'+(Math.floor(-data.main.temp));
                temp='-'+temp.substr(temp.length-2,2);
              }
              dispData.push(data.dt_txt.substr(11,2)+data.weather[0].icon+String(data.weather[0].id)+temp);
            }
          }
          info.weather.condition=dispData.join(',');
          console.log('weather condition='+info.weather.condition);
          Pebble.sendAppMessage({ 'KEY_WEATHER_CONDITION': info.weather.condition });
        } else {
          console.log('weather format Error');
        }
      } else { console.log('weather rest Error. http_status='+req2.status); }
    }
  }
  req2.send(null);
  info.lastCheck.weather=Date.now();
}

function executeRainfall(pos){
  
  var req2 = new XMLHttpRequest();
  var weatherUrl = 'http://weather.olp.yahooapis.jp/v1/place?appid='+yahooAppId+'&coordinates='+pos.coords.longitude+','+pos.coords.latitude+'&output=json';
  console.log('rainfall open.. url='+weatherUrl);
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
          console.log('rainfall format Error');
        }
      } else { console.log('rainfall rest Error. http_status='+req2.status); }
      setTimeout(function(){console.log('weather setTimeout called.');executeWeather(pos);},0);
    }
  }
  req2.send(null);
  info.lastCheck.rainfall=Date.now();
  
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
            Pebble.sendAppMessage({ 'KEY_CITY': info.addr.city });
            console.log('sendAppMessage city!');
          
            setTimeout(function(){console.log('rainfall setTimeout called.');executeRainfall(pos);},0);
            return;
          }
        }
        console.log('geoapi Error');
        
      } else { console.log('geoapi Error status='+req.status); }
    }
  }
  req.send(null);
  
}

function locationSuccess(pos) {
  
//   pos={
//     coords: {
//       latitude: 35.5081053,
//       longitude: 139.6671164
//     }
//   };
  
  console.log('Location changed!!');
  console.log('lat= ' + pos.coords.latitude + ' lon= ' + pos.coords.longitude);
  if ( info.pos.latitude==pos.coords.latitude && info.pos.longitude==pos.coords.longitude ){
    console.log('unmoved. skip...');
    if ( info.lastCheck.rainfall + RAINFALL_CHECK_INTERVAL < Date.now() ){
      setTimeout(function(){console.log('weatcher setTimeout called.');executeRainfall(pos);},0);
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