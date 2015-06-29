#include <pebble.h>

static struct Base {
  Window *window;
  Layer *canvasLayer;
  Layer *minutesLayer;
  Layer *calendarLayer;
  BitmapLayer *weatherIcon1Layer;
  BitmapLayer *weatherIcon2Layer;
  BitmapLayer *weatherIcon3Layer;
  
  GFont calFont;
  
  GBitmap *weatherIcon01;
  GBitmap *weatherIcon02;
  GBitmap *weatherIcon03;
  GBitmap *weatherIcon04;
  GBitmap *weatherIcon09;
  GBitmap *weatherIcon10;
  GBitmap *weatherIcon11;
  GBitmap *weatherIcon13;
  GBitmap *weatherIcon50;
  
  char *location;
  int32_t rainfall;
  char *weatherCondition;
  
  time_t currentTime;
  
  AppSync sync;
  uint8_t syncBuffer[512];
  time_t lastSync;
  
  
} base;

#define KEY_CITY 5
#define KEY_RAINFALL 6
#define KEY_WEATHER_CONDITION 7

uint8_t minutesRadius=12;

int16_t sin_max=128;

int16_t sin_array[720] = { 
  0,1,2,3,4,5,6,7,8,10,11,12,13,14,15,16,17,18,20,21,22,23,24,25,26,27,28,29,30,32,33,34,35,36,37,38,39,40,41,42,43,44,45,46,47,48,50,51,52,53,54,55,56,57,58,59,60,61,62,63,64,64,65,66,67,68,69,70,71,72,73,74,75,76,77,77,78,79,80,81,82,83,83,84,85,86,87,88,88,89,90,91,92,92,93,94,95,95,96,97,98,98,99,100,100,101,102,102,103,104,104,105,106,106,107,107,108,109,109,110,110,111,111,112,113,113,114,114,115,115,116,116,116,117,117,118,118,119,119,119,120,120,121,121,121,122,122,122,123,123,123,123,124,124,124,124,125,125,125,125,126,126,126,126,126,126,127,127,127,127,127,127,127,127,127,127,127,127,127,127,128,127,127,127,127,127,127,127,127,127,127,127,127,127,127,126,126,126,126,126,126,125,125,125,125,124,124,124,124,123,123,123,123,122,122,122,121,121,121,120,120,119,119,119,118,118,117,117,116,116,116,115,115,114,114,113,113,112,111,111,110,110,109,109,108,107,107,106,106,105,104,104,103,102,102,101,100,100,99,98,98,97,96,95,95,94,93,92,92,91,90,89,88,88,87,86,85,84,83,83,82,81,80,79,78,77,77,76,75,74,73,72,71,70,69,68,67,66,65,64,64,63,62,61,60,59,58,57,56,55,54,53,52,51,50,48,47,46,45,44,43,42,41,40,39,38,37,36,35,34,33,32,30,29,28,27,26,25,24,23,22,21,20,18,17,16,15,14,13,12,11,10,8,7,6,5,4,3,2,1,0,-2,-3,-4,-5,-6,-7,-8,-9,-11,-12,-13,-14,-15,-16,-17,-18,-19,-21,-22,-23,-24,-25,-26,-27,-28,-29,-30,-31,-33,-34,-35,-36,-37,-38,-39,-40,-41,-42,-43,-44,-45,-46,-47,-48,-49,-51,-52,-53,-54,-55,-56,-57,-58,-59,-60,-61,-62,-63,-64,-64,-65,-66,-67,-68,-69,-70,-71,-72,-73,-74,-75,-76,-77,-78,-78,-79,-80,-81,-82,-83,-84,-84,-85,-86,-87,-88,-89,-89,-90,-91,-92,-93,-93,-94,-95,-96,-96,-97,-98,-99,-99,-100,-101,-101,-102,-103,-103,-104,-105,-105,-106,-107,-107,-108,-108,-109,-110,-110,-111,-111,-112,-112,-113,-114,-114,-115,-115,-116,-116,-117,-117,-117,-118,-118,-119,-119,-120,-120,-120,-121,-121,-122,-122,-122,-123,-123,-123,-124,-124,-124,-124,-125,-125,-125,-125,-126,-126,-126,-126,-127,-127,-127,-127,-127,-127,-128,-128,-128,-128,-128,-128,-128,-128,-128,-128,-128,-128,-128,-128,-128,-128,-128,-128,-128,-128,-128,-128,-128,-128,-128,-128,-128,-128,-128,-127,-127,-127,-127,-127,-127,-126,-126,-126,-126,-125,-125,-125,-125,-124,-124,-124,-124,-123,-123,-123,-122,-122,-122,-121,-121,-120,-120,-120,-119,-119,-118,-118,-117,-117,-117,-116,-116,-115,-115,-114,-114,-113,-112,-112,-111,-111,-110,-110,-109,-108,-108,-107,-107,-106,-105,-105,-104,-103,-103,-102,-101,-101,-100,-99,-99,-98,-97,-96,-96,-95,-94,-93,-93,-92,-91,-90,-89,-89,-88,-87,-86,-85,-84,-84,-83,-82,-81,-80,-79,-78,-78,-77,-76,-75,-74,-73,-72,-71,-70,-69,-68,-67,-66,-65,-65,-64,-63,-62,-61,-60,-59,-58,-57,-56,-55,-54,-53,-52,-51,-49,-48,-47,-46,-45,-44,-43,-42,-41,-40,-39,-38,-37,-36,-35,-34,-33,-31,-30,-29,-28,-27,-26,-25,-24,-23,-22,-21,-19,-18,-17,-16,-15,-14,-13,-12,-11,-9,-8,-7,-6,-5,-4,-3,-2
};

int16_t cos_array[720] = {
  128,127,127,127,127,127,127,127,127,127,127,127,127,127,127,126,126,126,126,126,126,125,125,125,125,124,124,124,124,123,123,123,123,122,122,122,121,121,121,120,120,119,119,119,118,118,117,117,116,116,116,115,115,114,114,113,113,112,111,111,110,110,109,109,108,107,107,106,106,105,104,104,103,102,102,101,100,100,99,98,98,97,96,95,95,94,93,92,92,91,90,89,88,88,87,86,85,84,83,83,82,81,80,79,78,77,77,76,75,74,73,72,71,70,69,68,67,66,65,64,64,63,62,61,60,59,58,57,56,55,54,53,52,51,50,48,47,46,45,44,43,42,41,40,39,38,37,36,35,34,33,32,30,29,28,27,26,25,24,23,22,21,20,18,17,16,15,14,13,12,11,10,8,7,6,5,4,3,2,1,0,-2,-3,-4,-5,-6,-7,-8,-9,-11,-12,-13,-14,-15,-16,-17,-18,-19,-21,-22,-23,-24,-25,-26,-27,-28,-29,-30,-31,-33,-34,-35,-36,-37,-38,-39,-40,-41,-42,-43,-44,-45,-46,-47,-48,-49,-51,-52,-53,-54,-55,-56,-57,-58,-59,-60,-61,-62,-63,-64,-64,-65,-66,-67,-68,-69,-70,-71,-72,-73,-74,-75,-76,-77,-78,-78,-79,-80,-81,-82,-83,-84,-84,-85,-86,-87,-88,-89,-89,-90,-91,-92,-93,-93,-94,-95,-96,-96,-97,-98,-99,-99,-100,-101,-101,-102,-103,-103,-104,-105,-105,-106,-107,-107,-108,-108,-109,-110,-110,-111,-111,-112,-112,-113,-114,-114,-115,-115,-116,-116,-117,-117,-117,-118,-118,-119,-119,-120,-120,-120,-121,-121,-122,-122,-122,-123,-123,-123,-124,-124,-124,-124,-125,-125,-125,-125,-126,-126,-126,-126,-127,-127,-127,-127,-127,-127,-128,-128,-128,-128,-128,-128,-128,-128,-128,-128,-128,-128,-128,-128,-128,-128,-128,-128,-128,-128,-128,-128,-128,-128,-128,-128,-128,-128,-128,-127,-127,-127,-127,-127,-127,-126,-126,-126,-126,-125,-125,-125,-125,-124,-124,-124,-124,-123,-123,-123,-122,-122,-122,-121,-121,-120,-120,-120,-119,-119,-118,-118,-117,-117,-117,-116,-116,-115,-115,-114,-114,-113,-112,-112,-111,-111,-110,-110,-109,-108,-108,-107,-107,-106,-105,-105,-104,-103,-103,-102,-101,-101,-100,-99,-99,-98,-97,-96,-96,-95,-94,-93,-93,-92,-91,-90,-89,-89,-88,-87,-86,-85,-84,-84,-83,-82,-81,-80,-79,-78,-78,-77,-76,-75,-74,-73,-72,-71,-70,-69,-68,-67,-66,-65,-65,-64,-63,-62,-61,-60,-59,-58,-57,-56,-55,-54,-53,-52,-51,-49,-48,-47,-46,-45,-44,-43,-42,-41,-40,-39,-38,-37,-36,-35,-34,-33,-31,-30,-29,-28,-27,-26,-25,-24,-23,-22,-21,-19,-18,-17,-16,-15,-14,-13,-12,-11,-9,-8,-7,-6,-5,-4,-3,-2,-1,1,2,3,4,5,6,7,8,10,11,12,13,14,15,16,17,18,20,21,22,23,24,25,26,27,28,29,30,32,33,34,35,36,37,38,39,40,41,42,43,44,45,46,47,48,50,51,52,53,54,55,56,57,58,59,60,61,62,63,64,64,65,66,67,68,69,70,71,72,73,74,75,76,77,77,78,79,80,81,82,83,83,84,85,86,87,88,88,89,90,91,92,92,93,94,95,95,96,97,98,98,99,100,100,101,102,102,103,104,104,105,106,106,107,107,108,109,109,110,110,111,111,112,113,113,114,114,115,115,116,116,116,117,117,118,118,119,119,119,120,120,121,121,121,122,122,122,123,123,123,123,124,124,124,124,125,125,125,125,126,126,126,126,126,126,127,127,127,127,127,127,127,127,127,127,127,127,127,127
};

// 0<=min<=720
static uint16_t clock2degree720(uint16_t min){
  if ( min<=3*60 ){
    return 180-min; // 90-min/2;
  } else {
    return 720-(min-180); // 360-(min/2-90); 
  }
}

static int16_t sin1000(uint16_t min){
  return sin_array[clock2degree720(min)];
}

static int16_t cos1000(uint16_t min){
  return cos_array[clock2degree720(min)];
}

static GPoint calcMinPos(GRect bounds, uint16_t min, uint8_t radius){
  
  int32_t cx = cos1000(min);
  int32_t sy = sin1000(min);
  
  int32_t bottomY = bounds.size.h/2-radius;
  int32_t topY = -bottomY;
  int32_t rightX = bounds.size.w/2-radius;
  int32_t leftX = -rightX;
  
  int32_t x;  
  if ( min==180 ){
    x = rightX;
  } else if ( min==540 ){
    x = leftX;
  } else if ( min>540 || min<180 ){
    x = cx*bottomY/sy;
  } else if ( min<540 && min>180 ) {
    x = cx*topY/sy;
  } else {
    x = 0;
  }
  if ( x<leftX ){
    x=leftX;
  } else if ( x>rightX ){
    x=rightX;
  }
  
  int32_t y;
  if ( min==0 ){
    y = topY;
  } else if ( min==360 ){
    y = bottomY;
  } else if ( min>0 && min<360 ){
    y = sy*leftX/cx;
  } else if ( min>360 && min<720 ){
    y = sy*rightX/cx;
  } else {
    y = 0;
  }
  
  if ( y<topY ){
    y=topY;
  } else if ( y>bottomY ){
    y=bottomY;
  }
  
  return GPoint(x+bounds.size.w/2,y+bounds.size.h/2);  
}

static void minutesLayer_update_proc(Layer *this_layer, GContext *ctx) {
  
  uint8_t radius = minutesRadius;
  struct tm *curTime = localtime(&(base.currentTime));
  uint16_t min = (curTime->tm_hour%12)*60+curTime->tm_min;
  char ch[100];
  strftime(ch,100,"%02M",curTime);
  
  GRect bounds = layer_get_bounds(this_layer);
//   graphics_context_set_fill_color(ctx,GColorFromRGBA(255,0,0,64));
//   graphics_fill_rect(ctx,bounds,0,GCornerNone);
  
  uint8_t stroke_width=2;
//   bounds=GRect(bounds.origin.x-1,bounds.origin.y-1,bounds.size.w-2,bounds.size.h-2);
  GPoint center = calcMinPos(bounds,min,radius);
  graphics_context_set_fill_color(ctx, GColorBlue);
  graphics_fill_circle(ctx,center,radius-stroke_width);
//   graphics_context_set_stroke_width(ctx, stroke_width);
//   graphics_context_set_stroke_color(ctx, GColorBlack);
//   graphics_context_set_antialiased(ctx, true);
//   graphics_draw_circle(ctx,center,radius);

  GRect textFrame = GRect(center.x-radius,center.y-radius,radius*2,radius*2 );
  graphics_context_set_text_color(ctx, GColorWhite);
  graphics_draw_text(ctx,ch,fonts_get_system_font(FONT_KEY_GOTHIC_18),
                     textFrame,
                    GTextOverflowModeTrailingEllipsis ,
                     GTextAlignmentCenter ,
                     NULL
                    );
}

static GBitmap *getWeatherIcon(uint8_t offset){
  
  uint8_t pos=offset*9;
  uint8_t icon=(base.weatherCondition[pos+2]-'0')*10+base.weatherCondition[pos+3]-'0';

  GBitmap *iconBitmap;
  switch (icon){
    case 1:
      iconBitmap=base.weatherIcon01;
      break;
    case 2:
      iconBitmap=base.weatherIcon02;
      break;
    case 3:
      iconBitmap=base.weatherIcon03;
      break;
    case 4:
      iconBitmap=base.weatherIcon04;
      break;
    case 9:
      iconBitmap=base.weatherIcon09;
      break;
    case 10:
      iconBitmap=base.weatherIcon10;
      break;
    case 11:
      iconBitmap=base.weatherIcon11;
      break;
    case 13:
      iconBitmap=base.weatherIcon13;
      break;
    case 50:
    default:
      iconBitmap=base.weatherIcon50;
      break;
  }
  return iconBitmap; 
}

static void canvasLayer_update_proc(Layer *this_layer, GContext *ctx) {
  
  APP_LOG(APP_LOG_LEVEL_INFO, "canvasLayer_update_proc start");
  
  GRect bounds = layer_get_bounds(this_layer);
  graphics_context_set_fill_color(ctx,GColorFromRGB(180,180,255));
  graphics_fill_rect(ctx,bounds,0,GCornerNone);

  struct tm *curTime = localtime(&(base.currentTime));
  uint8_t curHour = (curTime->tm_hour%12);
  GPoint center = GPoint(bounds.origin.x+bounds.size.w/2,bounds.origin.y+bounds.size.h/2);
  GPoint curp = calcMinPos(bounds,curHour*60,0);
  GPoint nextp = calcMinPos(bounds,((curHour+1)%12)*60,0);
  GPoint diagonalp = GPoint(curp.x+nextp.x-center.x,curp.y+nextp.y-center.y);
  GPoint plist[4] = {center,curp,diagonalp,nextp};
  GPathInfo pi = {
    .num_points = 4,
    .points = plist
  };
  GPath *path = gpath_create(&pi);
  graphics_context_set_fill_color(ctx, GColorFromRGB(255,32,32));
  gpath_draw_filled(ctx,path);
  
  graphics_context_set_stroke_color(ctx,GColorFromRGB(0,0,0));
  graphics_context_set_stroke_width(ctx,2);
  for ( uint16_t hour=0 ; hour<12 ; hour++ ){
    uint16_t min=hour*60;
    GPoint p = calcMinPos(bounds,min,0);
    graphics_draw_line(ctx,p,GPoint(bounds.origin.x+bounds.size.w/2,bounds.origin.y+bounds.size.h/2));    
  }
  
  graphics_context_set_fill_color(ctx,GColorWhite);
  bounds.size.w = bounds.size.w-minutesRadius*2;
  bounds.size.h = bounds.size.h-minutesRadius*2;
  bounds.origin.x = bounds.origin.x+minutesRadius;  
  bounds.origin.y = bounds.origin.y+minutesRadius;  
  graphics_fill_rect(ctx,bounds,0,GCornerNone);
  
  graphics_context_set_text_color(ctx, GColorBlack);
  GRect textFrame = GRect(20,16,100,20);
      graphics_draw_text(ctx,base.location,fonts_get_system_font(FONT_KEY_GOTHIC_18),
                     textFrame,
                     GTextOverflowModeTrailingEllipsis ,
                     GTextAlignmentCenter ,
                     NULL
                    );  

  char hour1[32];
  bitmap_layer_set_bitmap(base.weatherIcon1Layer,getWeatherIcon(0));
  GRect textHour1 = GRect(16,75,20,20);
  strncpy(hour1,base.weatherCondition,2); hour1[2]='\0';
  graphics_draw_text(ctx,hour1,base.calFont,
                     textHour1,
                     GTextOverflowModeTrailingEllipsis ,
                     GTextAlignmentCenter ,
                     NULL
                    );
  
  char hour2[32];
  bitmap_layer_set_bitmap(base.weatherIcon2Layer,getWeatherIcon(1));
  GRect textHour2 = GRect(54,75,20,20);
  strncpy(hour2,base.weatherCondition+9,2); hour2[2]='\0';
  graphics_draw_text(ctx,hour2,base.calFont,
                     textHour2,
                     GTextOverflowModeTrailingEllipsis ,
                     GTextAlignmentCenter ,
                     NULL
                    );
  
  char hour3[32];
  GRect textHour3 = GRect(92,75,20,20);
  strncpy(hour3,base.weatherCondition+18,2);  hour3[2]='\0';
  graphics_draw_text(ctx,hour3,base.calFont,
                     textHour3,
                     GTextOverflowModeTrailingEllipsis ,
                     GTextAlignmentCenter ,
                     NULL
                    );
  bitmap_layer_set_bitmap(base.weatherIcon3Layer,getWeatherIcon(2));
  
  GRect textFrame2 = GRect(20,85,100,10);
  char rainFallStr[64];
  snprintf(rainFallStr,sizeof(rainFallStr),"rf: %ld, last: %ld",base.rainfall,(time(NULL)-base.lastSync)/60);
      graphics_draw_text(ctx,rainFallStr,fonts_get_system_font(FONT_KEY_GOTHIC_14),
                     textFrame2,
                     GTextOverflowModeTrailingEllipsis ,
                     GTextAlignmentCenter ,
                     NULL
                    );
}
  
static void calendarLayer_update_proc(Layer *this_layer, GContext *ctx) {
  
  time_t curTime = time(NULL);
  struct tm *today = localtime(&curTime);
  
  uint8_t today_pos = today->tm_wday+7;
  time_t t = curTime - (today_pos)*60*60*24;
  uint8_t calData[22];
  for ( uint8_t i=0 ; i<21 ; i++ ){
    struct tm *cd = localtime(&t);
    calData[i]=cd->tm_mday;
    t += 60*60*24;
  }
  
  GRect bounds = layer_get_bounds(this_layer);
  
  uint8_t bx=20;
  uint8_t by=110;
  uint8_t xpitch=15;
  uint8_t cpitch=6;
  uint8_t ypitch=12;
  char ch[16];
  
  graphics_context_set_text_color(ctx, GColorBlack);
  
  for ( uint8_t y=0 ; y<3 ; y++ ){
    for ( uint8_t x=0 ; x<7 ; x++ ){
 
      if ( today_pos==x+y*7 ){
        graphics_context_set_stroke_width(ctx,1);
        graphics_context_set_stroke_color(ctx, GColorBlack);
        graphics_draw_line(ctx,
                           GPoint(bx+x*xpitch,by+(y+1)*ypitch-2),
                           GPoint(bx+x*xpitch+cpitch*2+1,by+(y+1)*ypitch-2));
      }
      
      uint8_t mday = calData[x+y*7];
      snprintf(ch,16,"%d",mday/10);
      GRect textFrame1 = GRect(bx+x*xpitch,by+y*ypitch,8,16);

      graphics_draw_text(ctx,ch,base.calFont,
                     textFrame1,
                     GTextOverflowModeTrailingEllipsis ,
                     GTextAlignmentCenter ,
                     NULL
                    );
      
      snprintf(ch,16,"%d",mday%10);
      GRect textFrame2 = GRect(bx+x*xpitch+cpitch,by+y*ypitch,8,16);
      graphics_draw_text(ctx,ch,base.calFont,
                     textFrame2,
                     GTextOverflowModeTrailingEllipsis ,
                     GTextAlignmentCenter ,
                     NULL
                    );
    }
  }
}

static void main_window_load(Window *window){
  Layer *window_layer = window_get_root_layer(window);
  GRect window_bounds = layer_get_bounds(window_layer);
  
  base.canvasLayer = layer_create(GRect(0, 0, window_bounds.size.w, window_bounds.size.h));
  layer_add_child(window_layer, base.canvasLayer);
  layer_set_update_proc(base.canvasLayer, canvasLayer_update_proc);
  
  base.calendarLayer = layer_create(GRect(0, 0, window_bounds.size.w, window_bounds.size.h));
  layer_add_child(base.canvasLayer, base.calendarLayer);
  layer_set_update_proc(base.calendarLayer, calendarLayer_update_proc);
  
  base.minutesLayer = layer_create(GRect(0, 0, window_bounds.size.w, window_bounds.size.h));
  layer_add_child(base.calendarLayer,base.minutesLayer);
  layer_set_update_proc(base.minutesLayer, minutesLayer_update_proc);
  
  base.calFont = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_TEMPESTA_SEVEN_8));
  
  base.weatherIcon01 = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_WEATHER_SUN_RAYS_SMALL_01);
  base.weatherIcon02 = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_WEATHER_SUN_RAYS_SMALL_CLOUD_02);
  base.weatherIcon03 = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_WEATHER_CLOUD_03);
  base.weatherIcon04 = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_WEATHER_CLOUD_DARK_04);
  base.weatherIcon09 = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_WEATHER_CLOUD_DARK_RAIN_09);
  base.weatherIcon10 = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_WEATHER_SUN_RAY_CLOUD_DRIZZLE_10);
  base.weatherIcon11 = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_WEATHER_CLOUD_DARK_MULTIPLE_LIGHTNING_11);
  base.weatherIcon13 = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_WEATHER_CLOUD_DARK_SNOW_13);
  base.weatherIcon50 = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_WEATHER_CLOUD_DARK_FOG_50);
  
  base.weatherIcon1Layer = bitmap_layer_create(GRect(16, 36, 36, 36));
  layer_add_child(base.canvasLayer,bitmap_layer_get_layer(base.weatherIcon1Layer));
  bitmap_layer_set_compositing_mode(base.weatherIcon1Layer,GCompOpSet);
  
  base.weatherIcon2Layer = bitmap_layer_create(GRect(54, 36, 36, 36));
  layer_add_child(base.canvasLayer,bitmap_layer_get_layer(base.weatherIcon2Layer));
  bitmap_layer_set_compositing_mode(base.weatherIcon2Layer,GCompOpSet);
  
  base.weatherIcon3Layer = bitmap_layer_create(GRect(92, 36, 36, 36));
  layer_add_child(base.canvasLayer,bitmap_layer_get_layer(base.weatherIcon3Layer));
  bitmap_layer_set_compositing_mode(base.weatherIcon3Layer,GCompOpSet);
  
}

static void main_window_unload(Window *window){
  fonts_unload_custom_font(base.calFont);
  gbitmap_destroy(base.weatherIcon01);
  gbitmap_destroy(base.weatherIcon02);
  gbitmap_destroy(base.weatherIcon03);
  gbitmap_destroy(base.weatherIcon09);
  gbitmap_destroy(base.weatherIcon10);
  gbitmap_destroy(base.weatherIcon11);
  gbitmap_destroy(base.weatherIcon13);
  gbitmap_destroy(base.weatherIcon50);
  layer_destroy(base.minutesLayer);
  bitmap_layer_destroy(base.weatherIcon1Layer);
  bitmap_layer_destroy(base.weatherIcon2Layer);
  bitmap_layer_destroy(base.weatherIcon3Layer);
  layer_destroy(base.calendarLayer);
  layer_destroy(base.canvasLayer);
}

static void tick_handler(struct tm *tick_time, TimeUnits units_changed) {
  
  base.currentTime = time(NULL);
  layer_mark_dirty(base.minutesLayer);

}

static void sync_changed_handler(const uint32_t key, const Tuple *new_tuple, const Tuple *old_tuple, void *context) {
  
  switch(key){
    case KEY_CITY:
      APP_LOG(APP_LOG_LEVEL_INFO, "KEY_CITY received with value %s", (char*)new_tuple->value->cstring );
      base.location=(char*)new_tuple->value->cstring;
      base.lastSync=time(NULL);
      layer_mark_dirty(base.canvasLayer);
      break;
    case KEY_RAINFALL:
      APP_LOG(APP_LOG_LEVEL_INFO, "KEY_RAIN received with value %ld", (int32_t)new_tuple->value->int32 );
      base.rainfall=(int32_t)new_tuple->value->int32;
      base.lastSync=time(NULL);
      layer_mark_dirty(base.canvasLayer);
      break;
    case KEY_WEATHER_CONDITION:
      APP_LOG(APP_LOG_LEVEL_INFO, "KEY_WEATHER_CONDITION received with value %s", (char*)new_tuple->value->cstring );
      base.weatherCondition=(char*)new_tuple->value->cstring;
      base.lastSync=time(NULL);
      layer_mark_dirty(base.canvasLayer);
      break;
  }
  
}

static void sync_error_handler(DictionaryResult dict_error, AppMessageResult app_message_error, void *context) {
  APP_LOG(APP_LOG_LEVEL_ERROR, "sync error!");
}

void handle_init(void) {
  
  base.location="(init)";
  base.weatherCondition="__01d800,__01d800,__01d800";
  base.lastSync=0;
  
  app_message_open(app_message_inbox_size_maximum(), app_message_outbox_size_maximum());

  // Setup initial values
  Tuplet initial_values[] = {
    TupletInteger(KEY_RAINFALL, 0),
    TupletCString(KEY_CITY, "Loading.."),
    TupletCString(KEY_WEATHER_CONDITION, "__01d800,__01d800,__01d800")
  };
  
  // Begin using AppSync
  app_sync_init(&base.sync, base.syncBuffer, sizeof(base.syncBuffer), 
                initial_values, ARRAY_LENGTH(initial_values), 
                sync_changed_handler, sync_error_handler, NULL);
  
  base.window = window_create();

  APP_LOG(APP_LOG_LEVEL_INFO, "INIT!");
  
  window_set_window_handlers(base.window, (WindowHandlers) {
    .load = main_window_load,
    .unload = main_window_unload,
  });
  window_stack_push(base.window, true);  
  
  tick_timer_service_subscribe(MINUTE_UNIT, tick_handler);  
}
  
void handle_deinit(void) {
  app_sync_deinit(&base.sync);
  window_destroy(base.window);
}

int main(void) {
  handle_init();
  app_event_loop();
  handle_deinit();
}
