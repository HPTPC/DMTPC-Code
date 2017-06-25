//Dmtpc interface to flot
//
//Cosmin Deaconu <cozzyd@mit.edu>
//

DmtpcGraph.Instances = new Array(); 
DmtpcGraph.DefaultHours = 5; 

DmtpcGraph.zeropad = function(n, str)
{
  while (str.length < n)
  {
    str = "0" + str; 
  }

  return str; 
}


function DmtpcGraph(name, params)
{

  this.id = DmtpcGraph.Instances.length; 
  DmtpcGraph.Instances[this.id] = this; 
  
  this.table_names = params.table_names == null ? [] : params.table_names; 
  this.periodical;  

  this.has_min = false; 
  this.has_max = false; 
  this.nEvery= 1; 

//  this.ylabel =  params.ylabel == null ? "" : params.ylabel; 
  this.startstring="";
  this.endstring="";

  if (params.min!=null){ this.setMinValue(params.min);}   
  if (params.max!=null){ this.setMaxValue(params.max);} 

  this.placeholder = name; 
  this.noData(); 
}


DmtpcGraph.prototype.noData = function()
{
  $("#"+this.placeholder).html("Loading..."); 
}

DmtpcGraph.dataMinMax = function (data)
{
  var max = 0; 
  var min = 0; 
  var max_init = false; 
  var min_init = false; 
  for (var i = 0; i < data.length; i++)
  {
    for (var j = 0; j < data[i].data.length; j++)
    {
      var this_val = data[i].data[j][1]; 
      if (!max_init || this_val > max)
      {
        max_init = true; 
        max = this_val; 
      }
      if (!min_init || this_val < min)
      {
        min_init = true; 
        min = this_val; 
      }
    }
  }
  return [min,max]; 
}

DmtpcGraph.prototype.loadData = function()
{
 var counter = 0; 
 var placeholder = $("#"+this.placeholder); 
 var len = this.table_names.length; 

 var has_min = this.has_min; 
 var min = this.min_value; 
 var has_max = this.has_max; 
 var max = this.max_value; 

 var data = [] 

 for (var i =0; i < len;  i++)
 {

   var req_text = 'cgi-bin/read_data.py?which='+this.table_names[i]
                                               +this.startstring+this.endstring+"&nevery="+this.nEvery; 

     $.ajax({url: req_text, method: 'GET', dataType: 'json', success: 
       function(res)
       {
         data.push(res); 
         counter++; 
         if (counter == len) 
         {
           data.sort(function(a,b)
             {
                return a.label.localeCompare(b.label); 
             });
           var axis_options = {}; 
           minMax = null;
           if (has_min || has_max) minMax = DmtpcGraph.dataMinMax(data); 
           if (has_min && minMax[0] < min) { axis_options["min"] = min; }
           if (has_max && minMax[1] > max) { axis_options["max"] = max; }
           $.plot(placeholder, data, {lines: {show: false,lineWidth: 0}, points: {show: true, radius: 0.15},xaxis: { mode: "time"}, yaxis: axis_options, legend: {position:"nw"}});
         }
       }}); 
  }
}

DmtpcGraph.prototype.addTable = function(t)
{
  this.table_names.push(t);
}

DmtpcGraph.prototype.removeTable = function(t)
{
  while(true)
  {
    var i = this.table_names.indexOf(t);
    if (i < 0)
    {
      break;
    }
    this.table_names.splice(i,i);
  }
}

DmtpcGraph.prototype.setStartTime = function(year,month,day,hour,minute,second)
{
  this.startstring = "&start=" + DmtpcGraph.zeropad(4,year) 
                      + DmtpcGraph.zeropad(2,month)
                      + DmtpcGraph.zeropad(2,day)
                      + DmtpcGraph.zeropad(2,hour)
                      + DmtpcGraph.zeropad(2,minute)
                      + DmtpcGraph.zeropad(2,second);
}

DmtpcGraph.prototype.setEndTime = function(year,month,day,hour,minute,second)
{ 
  this.endstring = "&end=" + DmtpcGraph.zeropad(4,year) 
                           + DmtpcGraph.zeropad(2,month) 
                           + DmtpcGraph.zeropad(2,day) 
                           + DmtpcGraph.zeropad(2,hour) 
                           + DmtpcGraph.zeropad(2,minute) 
                           + DmtpcGraph.zeropad(2,second) ;
                            
}

DmtpcGraph.prototype.setMinValue = function(value)
{
  this.has_min = true; 
  this.min_value = value; 
}

DmtpcGraph.prototype.setMaxValue = function(value)
{
  this.has_max = true; 
  this.max_value = value; 
}


DmtpcGraph.prototype.UnsetMax = function()
{
  this.has_max = false; 
}

DmtpcGraph.prototype.UnsetMin = function()
{
  this.has_min = false; 
}

DmtpcGraph.prototype.setNEvery = function(nevery)
{
  if (nevery > 0)
  {
    this.nEvery = nevery; 
  }
}


DmtpcGraph.prototype.start = function(interval)
{
  this.periodical = setInterval('DmtpcGraph.Instances['+this.id+'].loadData()', interval); 
}

DmtpcGraph.prototype.stop = function()
{
  if (this.periodical) 
  {
    clearInterval(this.periodical);
  }
}


