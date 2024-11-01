var detectedDevices = null;

function scanWiFis(){
    $.getJSON('/api/v1/scan')
    .done(function(data){
        $("#networks").html('');
        data.networks.forEach(nw => {
            $("#networks").append(`<tr><td><button type="button" onclick="$('#form_ssid').val('${nw.ssid}');">${nw.auth?'(*)':''}${nw.ssid}</button></td><td>${nw.rssi}dBm</td></tr>`);
        });
    });
}
function saveInfo() {
    $.post("/api/v1/info", {
        name: $("#deviceName").val(),
        mqtt: $("#mqtt").val(),
        imps: $("#imps").val(),
        rts: $("#rts").val(),
        crc: $("#crc").is(":checked") ? "disabled" : "verify",
        autoFota: $("#autoFota").is(":checked") ? "disabled" : "auto",
        pullup: $("#pullup").is(":checked") ? "disabled" : "internal",
        m: $("#blinkHan").is(":checked") ? 0x0B : 0x0F,
        txPwr: $("#txPwr").val(),
    });
}

function fota() {
    if ($("#fota_md5").val().length == 32 && confirm("Are you sure?")) {
        $.post("/api/v1/fota", {
            url: $("#fota_url").val(),
            md5: $("#fota_md5").val(),
        });
    }
}

function _sourceOptions(sources, chosen) {
    let idx = [0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15];
    return idx.slice(0, sources.length).map(ds => `<option value="${ds}" ${ds == chosen ? "selected" : ""}>${sources[ds]}</option>`).join("");
}

function _stateOptions(states, chosen) {
    return states.map(ds => `<option value="${ds.id}" ${ds.id == chosen ? "selected" : ""}>${ds.name}</option>`).join("");
}

function deviceRow(dd, data) {
    return `<tr class="device"><td><input type="text" name="ddname" value="${dd.name}" placeholder="e.g. Fridge" minlength="1" maxlength="20" required form="device_${dd.id}"/></td>` +
    `<td><form method="post" action="/api/v1/device" id="device_${dd.id}"><input type="hidden" name="ddid" value="${dd.id}" form="device_${dd.id}"/></form>` +
        `<table><tr>` +
        `<td>${dd.id}</td>` +
        `<td><select name="ddsource" form="device_${dd.id}">${_sourceOptions(data.sources, dd.source)}</select></td>` +
        `<td>${dd.profileId}</td>` +
        `</tr></table></tr>` +
        `<tr class="device"><td/><td>${deleteForm(dd)}<table class="detection state"><tbody id="states_${dd.id}"><tr class="state"><td>States</td><td>power (W)</td></tr></tbody>` +
        `</table></td></tr>`;
}
function stateRow(dd, ds, data) {
    return `<tr class="state"><td><form method="post" action="/api/v1/state" id="state_${dd.id}_${ds.id}"><input type="hidden" name="ddid" value="${dd.id}" form="state_${dd.id}_${ds.id}"/></form>` +
        `<input type="text" name="dsname" value="${ds.name}" minlength="1" maxlength="20" required placeholder="off, on, full" form="state_${dd.id}_${ds.id}"/></td>` +
        `<td><table><tr>` +
        `<td><input type="number" name="dspower" value="${ds.power}" maxlength="6" size="6" required placeholder="125" form="state_${dd.id}_${ds.id}"/></td>` +
        `<td><select name="dsstatus" form="state_${dd.id}_${ds.id}"><option value="false" ${ds.status ? "":"selected"}>OFF</option><option value="true" ${ds.status ? "selected":""}>ON</option></select></td>` +
        `<td><input type="checkbox" id="push_${dd.id}_${ds.id}" name="dspush" value="true" ${ds.push ? "checked" : ""} form="state_${dd.id}_${ds.id}"/><label for="push_${dd.id}_${ds.id}">Push</label></td>` +
        `</tr><tr><td>minPower: ${ds.minPower}</td><td>timeout: ${ds.timeout}</td>` +
        `</tr></table></td></tr><tr><td/><td>${deleteForm(dd, ds)}<table class="detection trans"><tbody id="trans_${dd.id}_${ds.id}"><tr><td>Transitions</td><td>phase</td><td>to state</td></tr></tbody>` +
        `</table></td></tr>`;
}
function transRow(dd, ds, st, data) {
    return `<tr class="trans"><td><form method="post" action="/api/v1/transition" id="trans_${dd.id}_${ds.id}_${st.id}"><input type="hidden" name="ddid" value="${dd.id}" form="trans_${dd.id}_${ds.id}_${st.id}"/><input type="hidden" name="dsid" value="${ds.id}" form="trans_${dd.id}_${ds.id}_${st.id}"/></form>` +
        `<input type="number" id="dtdelta" name="dtdelta" value="${st.delta}" required placeholder="-98" form="trans_${dd.id}_${ds.id}_${st.id}"/></td>` +
        `<td><select id="dtsource" name="dtsource" form="trans_${dd.id}_${ds.id}_${st.id}">${_sourceOptions(data.sources, st.source)}</select></td>` +
        `<td><select id="dtto" name="dtto" form="trans_${dd.id}_${ds.id}_${st.id}">${_stateOptions(dd.states, st.toState)}</select></td>` +
        `</tr>`;
}

function createStateForm(dd) {
    return `<tr><td>+ State</td></tr>` +
        `<tr><td><form method="post" action="/api/v1/state" id="create_state_${dd.id}"><input type="hidden" name="ddid" value="${dd.id}"/></form>` +
        `<input type="text" name="dsname" minlength="1" maxlength="20" required placeholder="off, on, full" form="create_state_${dd.id}"/></td>` +
        `<td><table><tr><td><input type="number" name="dspower" maxlength="6" size="6" required placeholder="125W" form="create_state_${dd.id}"/></td>` +
        `<td><select name="dsstatus" form="create_state_${dd.id}"><option value="false" ${dd.states.length ? "":"selected"}>OFF</option><option value="true" ${dd.states.length ? "selected":""}>ON</option></select></td>` +
        `<td><input type="submit" value="Create State in ${dd.name}" form="create_state_${dd.id}"/></td>` +
        `</tr></table></td></tr>`;
}
function createTransForm(dd, ds, data) {
    var sourceSelect = `<select name="dtsource" form="create_trans_${dd.id}_${ds.id}">${_sourceOptions(data.sources, dd.source)}</select>`;
    return `<td><form method="post" action="/api/v1/transition" id="create_trans_${dd.id}_${ds.id}"><input type="hidden" name="ddid" value="${dd.id}"/><input type="hidden" name="dsid" value="${ds.id}"/></form>` +
    `<input type="number" name="dtdelta" required placeholder="-98W" form="create_trans_${dd.id}_${ds.id}"/></td>` +
    `<td>${sourceSelect}</td>` +
    `<td><select name="dtto" form="create_trans_${dd.id}_${ds.id}">${_stateOptions(dd.states)}</select></td>` +
    `<td><input type="submit" style="background-color: #51C1DB;" value="Create Transition from ${ds.name}" form="create_trans_${dd.id}_${ds.id}"/></td>`;
}
function deleteForm(dd, ds) {
    return `<form method="post" action="/api/v1/deletedevice" id="delete_${dd.id}${ds ? "_" + ds.id : ""}" onsubmit="return confirm('Are you sure you want to delete the ${ds ? "transition" : "state"}?');">` +
    `<input type="hidden" name="ddid" value="${dd.id}"/><input type="hidden" name="${ds ? "dsid" : "foo"}" value="${ds ? ds.id : "-1"}"/></form>`;
}

function _populateDevices(data) {
    $("#ddsource").html(_sourceOptions(data.sources));
    $("#devices").html('');
    data.devices.forEach(dd => {
        $("#devices").append(deviceRow(dd, data));

        dd.states.forEach(ds => {
            $(`#states_${dd.id}`).append(stateRow(dd, ds, data));

            ds.transitions.forEach(st => {
                $(`#trans_${dd.id}_${ds.id}`).append(transRow(dd, ds, st, data));
            });
        });
    });
}

function getDevices(callback){
    $.getJSON('/api/v1/device')
    .done(callback)
    .fail(function() {
        callback({
            devices: [
            {
                id: "heat_pump",
                name: "Heat Pump",
                profileId: "ABC123",
                states: [{
                    id: 0,
                    name: "Active",
                    power: 320,
                    minPower: 240,
                    timeout: 3600,
                    status: true,
                    push: true,
                    transitions: [{
                        id: 0,
                        delta: -42,
                        toState: 0,
                        source: 1,
                    }],
                },
                ],
            }, {
                id: "wine_cooler",
                name: "Wine Cooler",
                states: [{
                     id: 0,
                     name: "Off",
                     power: 0,
                     minPower: 0,
                     timeout: 600,
                     status: false,
                     transitions: [],
                     }],
                source: 1,
            },
            ],
            sources: ["W", "L1"],
        });
    });
}

function savePreference(key, upt) {
    var value;
    switch (upt) {
    case 0: value = $(`#p_${key}`).is(':checked') ? "true" : "false"; break;
    default: value = $(`#p_${key}`).val(); break;
    }
    $.post("/api/v1/prefs", {
        key: key,
        value: value,
    });
}

function clearPreference(key) {
    $.ajax("/api/v1/prefs", {
        method: 'DELETE',
        data: {
            key: key,
        },
    });
}

function onPreferences(data) {
    $("#prefs").html('');
    const txPwrOptions = [{n:"19dBm",v:"76"},{n:"17dBm",v:"68"},{n:"15dBm",v:"60"},{n:"13dBm",v:"52"},{n:"11dBm",v:"44"},{n:"8.5dBm",v:"36"},{n:"7dBm",v:"28"},{n:"5dBm",v:"20"}];
    const modeOptions = [{n:"Detect",v:"15"},{n:"HAN only",v:"9"},{n:"BLINK only",v:"10"},{n:"BLINK+HAN",v:"11"}];
    const cloudOptions = [{n:"Full",v:"15"},{n:"Appliances",v:"2"},{n:"Local only",v:"0"}];
    data.prefs.forEach(p => {
        const pattern = undefined === p.pattern || null == p.pattern ? '' : `pattern="${p.pattern}"`;
        const placeholder = undefined === p.placeholder || null == p.placeholder ? '' : `placeholder="${p.placeholder}"`;
        const maxlength = undefined === p.maxlength || null == p.maxlength ? '' : `maxlength="${p.maxlength}"`;
        const input = (p.key === "txPwr") ? `<select id="p_${p.key}" value="${p.value}">${txPwrOptions.map((e,i) => `<option value="${e.v}" ${e.v==p.value ? 'selected' : ''}>${e.n}</option>`).join('')}</select>`
            : (p.key === "mode") ? `<select id="p_${p.key}" value="${p.value}">${modeOptions.map((e,i) => `<option value="${e.v}" ${e.v==p.value ? 'selected' : ''}>${e.n}</option>`).join('')}</select>`
            : (p.key === "cloud") ? `<select id="p_${p.key}" value="${p.value}">${cloudOptions.map((e,i) => `<option value="${e.v}" ${e.v==p.value ? 'selected' : ''}>${e.n}</option>`).join('')}</select>`
            : `<input id="p_${p.key}" type="${p.type}" value="${p.value}" name="${p.key}" ${placeholder} ${maxlength} ${pattern} ${0===p.upt && p.value ? 'checked="checked"' : ''}/>`;
        const saveButton = `<button id="s_${p.key}" onclick="savePreference('${p.key}',${p.upt});">Save</button>`;
        const clearButton = `<button id="c_${p.key}" onclick="clearPreference('${p.key}');">Default</button>`;
        const row = `<tr><th><label for="p_${p.key}">${p.name}</label></th><td>${input}</td><td>${saveButton}</td><td>${clearButton}</td></tr>`;
        $("#prefs").append(row);
    });
}

function listPreferences(callback) {
    $.getJSON('/api/v1/prefs')
    .done(function(data) {
        callback(data);
//    })
//    .fail(function() {
//        callback({
//            prefs: [
//            {key: "name",name: "Device name",type: "text",upt:7,value: "static mock"},
//            {key: "txPwr",name: "WiFi txPwr",type: "select",upt:9,value:"68",defVal:"76"},
//            {key: "cloud",name: "Cloud Mode",type: "select",upt:9,value:"2",defVal:"15"},
//            ],
//        });
    });
}

function restart() {
    $.post("/api/v1/restart");
}

function connectES() {

if (!!window.EventSource) {
  var source = new EventSource('/events');

  source.addEventListener('open', function(e) {
    $("#logs").prepend('Browser connected to device /events .\n');
  }, false);
  source.addEventListener('error', function(e) {
    $("#logs").prepend("Browser lost connection to device.\n");
  }, false);

  source.addEventListener('config', function(e) {
    var config = JSON.parse(e.data);
    $("#deviceId").html(config.id);
    var u = `http://currentlyone_${config.id}.local`;
    $("#url").html(`<a href="${u}">${u}</a>`);
    $("#deviceVersion").html(config.v);
    $("#mode").html(config.m);
    $("#ssid").html(config.ssid);
    $("#rssi").html(config.rssi);
    $("#localIP").html(config.localIP);
    $("#deviceName").val(config.name);
    $("#mqtt").val(config.mqtt);
    $("#imps").val(config.imps);
    $("#time").html(config.t);
    $("#logs").prepend(`config /events ${config.t}\n`);
    $("#crc").prop('checked', config.crc == "disabled");
    $("#autoFota").prop('checked', config.autoFota == "disabled");
    $("#pullup").prop('checked', config.pullup == "disabled");
    $("#blinkHan").prop('checked', config.m == "H+B");
    $("#txPwr").val(config.txPwr);
    if ($("#fota_url").val() === "") {
        $.ajax("https://collector.currently.cloud/api/v1/fota", {
            dataType: "jsonp",
            data: {
                deviceId: config.id,
            },
        })
        .done(function(data){
            $("#fota_url").val(data.url);
            $("#fota_md5").val(data.md5);
            $("#fota_version").html(data.version);
        });
    }
    listPreferences(onPreferences);
  }, false);
  source.addEventListener('state', function(e) {
    var state = JSON.parse(e.data);
    const localTime = new Date(state.sec * 1000).toLocaleString();
    if (null != state.W) {
        $("#W").html(state.W ? state.W : -state.P);
    }
    if (null != state.Wh) {
        $("#Wh").html(state.Wh);
    }
    if (null != state.bW) {
        $("#bW").html(state.bW);
    }
    if (null != state.W) {
        $("#mode").html(state.m);
    }
    $("#time").html(localTime);
    if (state.detected) {
        state.detected.forEach(ds => {
            $(`#since_${ds.id}`).html(localTime);
            $(`#state_${ds.id}`).html(detectedDevices.find(dd => dd.id==ds.id).states[ds.to].name);
            $(`#wh_${ds.id}`).html(`${ds.Wh} Wh`);
        });
    }
  }, false);
  source.addEventListener('status', function(e) {
    console.log("status", e.data);
    $("#status").html(e.data);
  }, false);
  source.addEventListener('ping', function(e) {
    console.log("ping", e.data);
    var ping = JSON.parse(e.data);
    $("#time").html(new Date(ping.sec * 1000).toLocaleString());
  }, false);
  source.addEventListener('log', function(e) {
    $("#logs").prepend(e.data + '\n');
  }, false);
  source.addEventListener('progress', function(e) {
    var progress = JSON.parse(e.data);
    $("#fota_progress").html(`${progress.v/1000} of ${progress.s/1000}`);
    $("#fota_progress").val(progress.v);
    $("#fota_progress").attr('max', progress.s);
  }, false);
  source.addEventListener('transition', function(e) {
    const trans = JSON.parse(e.data);
    const idPrefix = trans.id;
    $(`#since_${idPrefix}`).html(trans.t);
    $(`#state_${idPrefix}`).html(trans.sName);
    $(`#wh_${idPrefix}`).html(trans.Wh);
  }, false);
}
}

function loadDetected(data) {
detectedDevices = data.devices;
data.devices.forEach(dd => {
    $("#states").append(`<tr><td>${dd.name}</td><td id="since_${dd.id}">unknown</td><td id="state_${dd.id}">unknown</td><td id="wh_${dd.id}">unknown</td></tr>`);
});
}

function onIndex() {
    getDevices(loadDetected);
    connectES();
    listPreferences(onPreferences);
}