/****************************************************************************
 Copyright (c) 2013-2016 Chukong Technologies Inc.
 Copyright (c) 2017-2018 Xiamen Yaji Software Co., Ltd.

 http://www.cocos.com

 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated engine source code (the "Software"), a limited,
  worldwide, royalty-free, non-assignable, revocable and non-exclusive license
 to use Cocos Creator solely to develop games on your target platforms. You shall
  not use Cocos Creator software for developing other software or tools that's
  used for developing games. You are not granted to publish, distribute,
  sublicense, and/or sell copies of Cocos Creator.

 The software or tools in this License Agreement are licensed, not sold.
 Xiamen Yaji Software Co., Ltd. reserves all rights not expressly granted to you.

 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 THE SOFTWARE.
 ****************************************************************************/

/**
 * @module cc
 */

var MAX_POOL_SIZE = 20;

//data structures
/*
 * A list double-linked list used for "updates with priority"
 * @class ListEntry
 * @param {ListEntry} prev
 * @param {ListEntry} next
 * @param {function} callback
 * @param {Object} target not retained (retained by hashUpdateEntry)
 * @param {Number} priority
 * @param {Boolean} paused
 * @param {Boolean} markedForDeletion selector will no longer be called and entry will be removed at end of the next tick
 */
var ListEntry = function (prev, next, callback, target, priority, paused, markedForDeletion) {
    this.prev = prev;
    this.next = next;
    this.callback = callback;
    this.target = target;
    this.priority = priority;
    this.paused = paused;
    this.markedForDeletion = markedForDeletion;
    this.isUpdate = !callback;
};

var _listEntries = [];
ListEntry.get = function (prev, next, callback, target, priority, paused, markedForDeletion) {
    var result = _listEntries.pop();
    if (result) {
        result.prev = prev;
        result.next = next;
        result.callback = callback;
        result.target = target;
        result.priority = priority;
        result.paused = paused;
        result.markedForDeletion = markedForDeletion;
        result.isUpdate = !callback;
    }
    else {
        result = new ListEntry(prev, next, callback, target, priority, paused, markedForDeletion);
    }
    return result;
};
ListEntry.put = function (entry) {
    if (_listEntries.length < MAX_POOL_SIZE) {
        entry.prev = entry.next = entry.callback = entry.target = null;
        _listEntries.push(entry);
    }
};

/*
 * A update entry list
 * @class HashUpdateEntry
 * @param {Array} list Which list does it belong to ?
 * @param {ListEntry} entry entry in the list
 * @param {Object} target hash key (retained)
 * @param {function} callback
 */
var HashUpdateEntry = function (list, entry, target, callback) {
    this.list = list;
    this.entry = entry;
    this.target = target;
    this.callback = callback;
};
var _hashUpdateEntries = [];
HashUpdateEntry.get = function (list, entry, target, callback) {
    var result = _hashUpdateEntries.pop();
    if (result) {
        result.list = list;
        result.entry = entry;
        result.target = target;
        result.callback = callback;
    }
    else {
        result = new HashUpdateEntry(list, entry, target, callback);
    }
    return result;
};
HashUpdateEntry.put = function (entry) {
    if (_hashUpdateEntries.length < MAX_POOL_SIZE) {
        entry.list = entry.entry = entry.target = entry.callback = null;
        _hashUpdateEntries.push(entry);
    }
};

//
/*
 * Hash Element used for "selectors with interval"
 * @class HashTimerEntry
 * @param {Array} timers
 * @param {Object} target  hash key (retained)
 * @param {Number} timerIndex
 * @param {Timer} currentTimer
 * @param {Boolean} currentTimerSalvaged
 * @param {Boolean} paused
 */
var HashTimerEntry = function (timers, target, timerIndex, currentTimer, currentTimerSalvaged, paused) {
    var _t = this;
    _t.timers = timers;
    _t.target = target;
    _t.timerIndex = timerIndex;
    _t.currentTimer = currentTimer;
    _t.currentTimerSalvaged = currentTimerSalvaged;
    _t.paused = paused;
};
var _hashTimerEntries = [];
HashTimerEntry.get = function (timers, target, timerIndex, currentTimer, currentTimerSalvaged, paused) {
    var result = _hashTimerEntries.pop();
    if (result) {
        result.timers = timers;
        result.target = target;
        result.timerIndex = timerIndex;
        result.currentTimer = currentTimer;
        result.currentTimerSalvaged = currentTimerSalvaged;
        result.paused = paused;
    }
    else {
        result = new HashTimerEntry(timers, target, timerIndex, currentTimer, currentTimerSalvaged, paused);
    }
    return result;
};
HashTimerEntry.put = function (entry) {
    if (_hashTimerEntries.length < MAX_POOL_SIZE) {
        entry.timers = entry.target = entry.currentTimer = null;
        _hashTimerEntries.push(entry);
    }
};

/*
 * Light weight timer
 * @extends cc.Class
 */
function CallbackTimer () {
    this._lock = false;
    this._scheduler = null;
    this._elapsed = -1;
    this._runForever = false;
    this._useDelay = false;
    this._timesExecuted = 0;
    this._repeat = 0;
    this._delay = 0;
    this._interval = 0;

    this._target = null;
    this._callback = null;
}

var proto = CallbackTimer.prototype;

proto.initWithCallback = function (scheduler, callback, target, seconds, repeat, delay) {
    this._lock = false;
    this._scheduler = scheduler;
    this._target = target;
    this._callback = callback;

    this._elapsed = -1;
    this._interval = seconds;
    this._delay = delay;
    this._useDelay = (this._delay > 0);
    this._repeat = repeat;
    this._runForever = (this._repeat === cc.macro.REPEAT_FOREVER);
    return true;
};
/**
 * @return {Number} returns interval of timer
 */
proto.getInterval = function(){return this._interval;};
/**
 * @param {Number} interval set interval in seconds
 */
proto.setInterval = function(interval){this._interval = interval;};

/**
 * triggers the timer
 * @param {Number} dt delta time
 */
proto.update = function (dt) {
    if (this._elapsed === -1) {
        this._elapsed = 0;
        this._timesExecuted = 0;
    } else {
        this._elapsed += dt;
        if (this._runForever && !this._useDelay) {//standard timer usage
            if (this._elapsed >= this._interval) {
                this.trigger();
                this._elapsed = 0;
            }
        } else {//advanced usage
            if (this._useDelay) {
                if (this._elapsed >= this._delay) {
                    this.trigger();

                    this._elapsed -= this._delay;
                    this._timesExecuted += 1;
                    this._useDelay = false;
                }
            } else {
                if (this._elapsed >= this._interval) {
                    this.trigger();

                    this._elapsed = 0;
                    this._timesExecuted += 1;
                }
            }

            if (this._callback && !this._runForever && this._timesExecuted > this._repeat)
                this.cancel();
        }
    }
};

proto.getCallback = function(){
    return this._callback;
};

proto.trigger = function () {
    if (this._target && this._callback) {
        this._lock = true;
        this._callback.call(this._target, this._elapsed);
        this._lock = false;
    }
};

proto.cancel = function () {
    //override
    this._scheduler.unschedule(this._callback, this._target);
};

var _timers = [];
CallbackTimer.get = function () {
    return _timers.pop() || new CallbackTimer();
};
CallbackTimer.put = function (timer) {
    if (_timers.length < MAX_POOL_SIZE && !timer._lock) {
        timer._scheduler = timer._target = timer._callback = null;
        _timers.push(timer);
    }
};

var getTargetId = function (target) {
    return target.__instanceId || target.uuid;
};

/**
 * !#en
 * Scheduler is responsible of triggering the scheduled callbacks.<br/>
 * You should not use NSTimer. Instead use this class.<br/>
 * <br/>
 * There are 2 different types of callbacks (selectors):<br/>
 *     - update callback: the 'update' callback will be called every frame. You can customize the priority.<br/>
 *     - custom callback: A custom callback will be called every frame, or with a custom interval of time<br/>
 * <br/>
 * The 'custom selectors' should be avoided when possible. It is faster,
 * and consumes less memory to use the 'update callback'. *
 * !#zh
 * Scheduler ????????????<br/>
 * ?????,???? cc.director.getScheduler() ?????????<br/>
 * ???????????:<br/>
 *     - update ???:??????????????????<br/>
 *     - ??????:????????????????????????<br/>
 * ?????????,???? update ???,?? update ?????,??????????
 *
 * @class Scheduler
 */
cc.Scheduler = cc._Class.extend({

    ctor: function () {
        this._timeScale = 1.0;
        this._updatesNegList = [];  // list of priority < 0
        this._updates0List = [];    // list of priority == 0
        this._updatesPosList = [];  // list of priority > 0
        this._hashForUpdates = {};  // hash used to fetch quickly the list entries for pause, delete, etc
        this._hashForTimers = {};   // Used for "selectors with interval"
        this._currentTarget = null;
        this._currentTargetSalvaged = false;
        this._updateHashLocked = false; // If true unschedule will not remove anything from a hash. Elements will only be marked for deletion.

        this._arrayForTimers = [];  // Speed up indexing
        //this._arrayForUpdates = [];   // Speed up indexing
    },

    //-----------------------private method----------------------

    _removeHashElement: function (element) {
        delete this._hashForTimers[getTargetId(element.target)];
        var arr = this._arrayForTimers;
        for (var i = 0, l = arr.length; i < l; i++) {
            if (arr[i] === element) {
                arr.splice(i, 1);
                break;
            }
        }
        HashTimerEntry.put(element);
    },

    _removeUpdateFromHash: function (entry) {
        var targetId = getTargetId(entry.target);
        var self = this, element = self._hashForUpdates[targetId];
        if (element) {
            // Remove list entry from list
            var list = element.list, listEntry = element.entry;
            for (var i = 0, l = list.length; i < l; i++) {
                if (list[i] === listEntry) {
                    list.splice(i, 1);
                    break;
                }
            }

            delete self._hashForUpdates[targetId];
            ListEntry.put(listEntry);
            HashUpdateEntry.put(element);
        }
    },

    _priorityIn: function (ppList, listElement, priority) {
        for (var i = 0; i < ppList.length; i++){
            if (priority < ppList[i].priority) {
                ppList.splice(i, 0, listElement);
                return;
            }
        }
        ppList.push(listElement);
    },

    _appendIn: function (ppList, listElement) {
        ppList.push(listElement);
    },

    //-----------------------public method-------------------------
    /**
     * !#en
     * Modifies the time of all scheduled callbacks.<br/>
     * You can use this property to create a 'slow motion' or 'fast forward' effect.<br/>
     * Default is 1.0. To create a 'slow motion' effect, use values below 1.0.<br/>
     * To create a 'fast forward' effect, use values higher than 1.0.<br/>
     * Note:It will affect EVERY scheduled selector / action.
     * !#zh
     * ????????????<br/>
     * ?????????????? �slow motion(???)� ? �fast forward(??)� ????<br/>
     * ??? 1.0?????? �slow motion(???)� ??,????? 1.0?<br/>
     * ??? �fast forward(??)� ??,????? 1.0?<br/>
     * ??:???? Scheduler ??????????
     * @method setTimeScale
     * @param {Number} timeScale
     */
    setTimeScale: function (timeScale) {
        this._timeScale = timeScale;
    },

    /**
     * !#en Returns time scale of scheduler.
     * !#zh ????????????
     * @method getTimeScale
     * @return {Number}
     */
    getTimeScale: function () {
        return this._timeScale;
    },

    /**
     * !#en 'update' the scheduler. (You should NEVER call this method, unless you know what you are doing.)
     * !#zh update ?????(???????????,????????????)
     * @method update
     * @param {Number} dt delta time
     */
    update: function (dt) {
        this._updateHashLocked = true;
        if(this._timeScale !== 1)
            dt *= this._timeScale;

        var i, list, len, entry;

        for(i=0,list=this._updatesNegList, len = list.length; i<len; i++){
            entry = list[i];
            if (!entry.paused && !entry.markedForDeletion)
                entry.isUpdate ? entry.target.update(dt) : entry.callback.call(entry.target, dt);
        }

        for(i=0, list=this._updates0List, len=list.length; i<len; i++){
            entry = list[i];
            if (!entry.paused && !entry.markedForDeletion)
                entry.isUpdate ? entry.target.update(dt) : entry.callback.call(entry.target, dt);
        }

        for(i=0, list=this._updatesPosList, len=list.length; i<len; i++){
            entry = list[i];
            if (!entry.paused && !entry.markedForDeletion)
                entry.isUpdate ? entry.target.update(dt) : entry.callback.call(entry.target, dt);
        }

        // Iterate over all the custom selectors
        var elt, arr = this._arrayForTimers;
        for(i=0; i<arr.length; i++){
            elt = arr[i];
            this._currentTarget = elt;
            this._currentTargetSalvaged = false;

            if (!elt.paused){
                // The 'timers' array may change while inside this loop
                for (elt.timerIndex = 0; elt.timerIndex < elt.timers.length; ++(elt.timerIndex)){
                    elt.currentTimer = elt.timers[elt.timerIndex];
                    elt.currentTimerSalvaged = false;

                    elt.currentTimer.update(dt);
                    elt.currentTimer = null;
                }
            }

            // elt, at this moment, is still valid
            // so it is safe to ask this here (issue #490)
            //elt = elt.hh.next;

            // only delete currentTarget if no actions were scheduled during the cycle (issue #481)
            if (this._currentTargetSalvaged && this._currentTarget.timers.length === 0) {
                this._removeHashElement(this._currentTarget);
                --i;
            }
        }

        // delete all updates that are marked for deletion
        // updates with priority < 0
        for(i=0,list=this._updatesNegList; i<list.length; ){
            entry = list[i];
            if(entry.markedForDeletion)
                this._removeUpdateFromHash(entry);
            else
                i++;
        }

        for(i=0, list=this._updates0List; i<list.length; ){
            entry = list[i];
            if (entry.markedForDeletion)
                this._removeUpdateFromHash(entry);
            else
                i++;
        }

        for(i=0, list=this._updatesPosList; i<list.length; ){
            entry = list[i];
            if (entry.markedForDeletion)
                this._removeUpdateFromHash(entry);
            else
                i++;
        }

        this._updateHashLocked = false;
        this._currentTarget = null;
    },

    /**
     * !#en
     * <p>
     *   The scheduled method will be called every 'interval' seconds.</br>
     *   If paused is YES, then it won't be called until it is resumed.<br/>
     *   If 'interval' is 0, it will be called every frame, but if so, it recommended to use 'scheduleUpdateForTarget:' instead.<br/>
     *   If the callback function is already scheduled, then only the interval parameter will be updated without re-scheduling it again.<br/>
     *   repeat let the action be repeated repeat + 1 times, use cc.macro.REPEAT_FOREVER to let the action run continuously<br/>
     *   delay is the amount of time the action will wait before it'll start<br/>
     * </p>
     * !#zh
     * ??????,??????????????????</br>
     * ???????????,?????????????</br>
     * ?? paused ?? true,???? resume ?????????</br>
     * ?? interval ?? 0,??????????????,??????,
     * ???? scheduleUpdateForTarget ???</br>
     * ??????????????,??????????????????,??????????<br/>
     * repeat ????????? repeat + 1 ?,?? cc.macro.REPEAT_FOREVER
     * ?????????????<br/>
     * delay ???????,???????????????????
     * @method scheduleCallbackForTarget
     * @deprecated since v3.4 please use .schedule
     * @param {Object} target
     * @param {Function} callback_fn
     * @param {Number} interval
     * @param {Number} [repeat=cc.macro.REPEAT_FOREVER]
     * @param {Number} [delay=0]
     * @param {Boolean} paused
     * @example {@link cocos2d/core/CCScheduler/scheduleCallbackForTarget.js}
     * @typescript
     * scheduleCallbackForTarget(target: any, callback: Function, interval: number, repeat: number, delay: number, paused?: boolean): void
     * scheduleCallbackForTarget(target: any, callback: Function, interval: number, paused?: boolean): void
     */
    scheduleCallbackForTarget: function(target, callback_fn, interval, repeat, delay, paused){
        //cc.log("scheduleCallbackForTarget is deprecated. Please use schedule.");
        this.schedule(callback_fn, target, interval, repeat, delay, paused);
    },

    /**
     * !#en The schedule
     * !#zh ???
     * @method schedule
     * @param {Function} callback
     * @param {Object} target
     * @param {Number} interval
     * @param {Number} [repeat=cc.macro.REPEAT_FOREVER]
     * @param {Number} [delay=0]
     * @param {Boolean} paused
     * @example {@link cocos2d/core/CCScheduler/schedule.js}
     * @typescript
     * schedule(callback: Function, target: any, interval: number, repeat: number, delay: number, paused?: boolean): void
     * schedule(callback: Function, target: any, interval: number, paused?: boolean): void
     */
    schedule: function (callback, target, interval, repeat, delay, paused) {
        'use strict';
        if (typeof callback !== 'function') {
            var tmp = callback;
            callback = target;
            target = tmp;
        }
        //selector, target, interval, repeat, delay, paused
        //selector, target, interval, paused
        if (arguments.length === 4 || arguments.length === 5) {
            paused = !!repeat;
            repeat = cc.macro.REPEAT_FOREVER;
            delay = 0;
        }

        cc.assertID(target, 1502);

        var instanceId = getTargetId(target);
        cc.assertID(instanceId, 1510);
        var element = this._hashForTimers[instanceId];
        if (!element) {
            // Is this the 1st element ? Then set the pause level to all the callback_fns of this target
            element = HashTimerEntry.get(null, target, 0, null, null, paused);
            this._arrayForTimers.push(element);
            this._hashForTimers[instanceId] = element;
        } else if (element.paused !== paused) {
            cc.warnID(1511);
        }

        var timer, i;
        if (element.timers == null) {
            element.timers = [];
        }
        else {
            for (i = 0; i < element.timers.length; ++i) {
                timer = element.timers[i];
                if (timer && callback === timer._callback) {
                    cc.logID(1507, timer.getInterval(), interval);
                    timer._interval = interval;
                    return;
                }
            }
        }

        timer = CallbackTimer.get();
        timer.initWithCallback(this, callback, target, interval, repeat, delay);
        element.timers.push(timer);

        if (this._currentTarget === element && this._currentTargetSalvaged) {
            this._currentTargetSalvaged = false;
        }
    },

    /**
     * !#en
     * Schedules the update callback for a given target,
     * the callback will be invoked every frame after schedule started.
     * !#zh
     * ???????????????? update ????
     * update ???????????????????,??????????
     * @method scheduleUpdate
     * @param {Object} target
     * @param {Number} priority
     * @param {Boolean} paused
     * @param {Function} updateFunc
     */
    scheduleUpdate: function(target, priority, paused, updateFunc) {
        var targetId = getTargetId(target);
        cc.assertID(targetId, 1510);
        var hashElement = this._hashForUpdates[targetId];
        if (hashElement && hashElement.entry){
            // check if priority has changed
            if (hashElement.entry.priority !== priority){
                if (this._updateHashLocked){
                    cc.logID(1506);
                    hashElement.entry.markedForDeletion = false;
                    hashElement.entry.paused = paused;
                    return;
                }else{
                    // will be added again outside if (hashElement).
                    this.unscheduleUpdate(target);
                }
            }else{
                hashElement.entry.markedForDeletion = false;
                hashElement.entry.paused = paused;
                return;
            }
        }

        if (updateFunc) {
            cc.warnID(1512);
        }

        var listElement = ListEntry.get(null, null, updateFunc, target, priority, paused, false);
        var ppList;

        // most of the updates are going to be 0, that's way there
        // is an special list for updates with priority 0
        if (priority === 0) {
            ppList = this._updates0List;
            this._appendIn(ppList, listElement);
        }
        else {
            ppList = priority < 0 ? this._updatesNegList : this._updatesPosList;
            this._priorityIn(ppList, listElement, priority);
        }

        //update hash entry for quick access
        this._hashForUpdates[targetId] = HashUpdateEntry.get(ppList, listElement, target, null);
    },

    /**
     * !#en
     * Unschedules a callback for a callback and a given target.
     * If you want to unschedule the "update", use `unscheduleUpdate()`
     * !#zh
     * ???????????????
     * ?????? update ???,??? unscheduleUpdate()?
     * @method unschedule
     * @param {Function} callback The callback to be unscheduled
     * @param {Object} target The target bound to the callback.
     */
    unschedule: function (callback, target) {
        //callback, target

        // explicity handle nil arguments when removing an object
        if (!target || !callback)
            return;
        var targetId = getTargetId(target);
        cc.assertID(targetId, 1510);

        var self = this, element = self._hashForTimers[targetId];
        if (element) {
            var timers = element.timers;
            for(var i = 0, li = timers.length; i < li; i++){
                var timer = timers[i];
                if (callback === timer._callback) {
                    if ((timer === element.currentTimer) && (!element.currentTimerSalvaged)) {
                        element.currentTimerSalvaged = true;
                    }
                    timers.splice(i, 1);
                    CallbackTimer.put(timer);
                    //update timerIndex in case we are in tick;, looping over the actions
                    if (element.timerIndex >= i) {
                        element.timerIndex--;
                    }

                    if (timers.length === 0) {
                        if (self._currentTarget === element) {
                            self._currentTargetSalvaged = true;
                        } else {
                            self._removeHashElement(element);
                        }
                    }
                    return;
                }
            }
        }
    },

    /** 
     * !#en Unschedules the update callback for a given target.
     * !#zh ??????? update ????
     * @method unscheduleUpdate
     * @param {Object} target The target to be unscheduled.
     */
    unscheduleUpdate: function (target) {
        if (!target)
            return;
        var targetId = getTargetId(target);
        cc.assertID(targetId, 1510);

        var element = this._hashForUpdates[targetId];
        if (element) {
            if (this._updateHashLocked) {
                element.entry.markedForDeletion = true;
            } else {
                this._removeUpdateFromHash(element.entry);
            }
        }
    },

    /** 
     * !#en
     * Unschedules all scheduled callbacks for a given target.
     * This also includes the "update" callback.
     * !#zh ????????????,?? update ????
     * @method unscheduleAllForTarget
     * @param {Object} target The target to be unscheduled.
     */
    unscheduleAllForTarget: function (target) {
        // explicit nullptr handling
        if (!target){
            return;
        }
        var targetId = getTargetId(target);
        cc.assertID(targetId, 1510);

        // Custom Selectors
        var element = this._hashForTimers[targetId];
        if (element) {
            var timers = element.timers;
            if (timers.indexOf(element.currentTimer) > -1 && 
                (!element.currentTimerSalvaged)) {
                element.currentTimerSalvaged = true;
            }
            for (var i = 0, l = timers.length; i < l; i++) {
                CallbackTimer.put(timers[i]);
            }
            timers.length = 0;

            if (this._currentTarget === element){
                this._currentTargetSalvaged = true;
            }else{
                this._removeHashElement(element);
            }
        }

        // update selector
        this.unscheduleUpdate(target);
    },

    /**
     * !#en
     * Unschedules all scheduled callbacks from all targets including the system callbacks.<br/>
     * You should NEVER call this method, unless you know what you are doing.
     * !#zh
     * ????????????,????????<br/>
     * ???????,???????????
     * @method unscheduleAll
     */
    unscheduleAll: function(){
        this.unscheduleAllWithMinPriority(cc.Scheduler.PRIORITY_SYSTEM);
    },

    /**
     * !#en
     * Unschedules all callbacks from all targets with a minimum priority.<br/>
     * You should only call this with `PRIORITY_NON_SYSTEM_MIN` or higher.
     * !#zh
     * ?????????????????????<br/>
     * ????????????? PRIORITY_NON_SYSTEM_MIN ?????
     * @method unscheduleAllWithMinPriority
     * @param {Number} minPriority The minimum priority of selector to be unscheduled. Which means, all selectors which
     *        priority is higher than minPriority will be unscheduled.
     */
    unscheduleAllWithMinPriority: function(minPriority){
        // Custom Selectors
        var i, element, arr = this._arrayForTimers;
        for(i=arr.length-1; i>=0; i--){
            element = arr[i];
            this.unscheduleAllForTarget(element.target);
        }

        // Updates selectors
        var entry;
        var temp_length = 0;
        if(minPriority < 0){
            for(i=0; i<this._updatesNegList.length; ){
                temp_length = this._updatesNegList.length;
                entry = this._updatesNegList[i];
                if(entry && entry.priority >= minPriority)
                    this.unscheduleUpdate(entry.target);
                if (temp_length == this._updatesNegList.length)
                    i++;
            }
        }

        if(minPriority <= 0){
            for(i=0; i<this._updates0List.length; ){
                temp_length = this._updates0List.length;
                entry = this._updates0List[i];
                if (entry)
                    this.unscheduleUpdate(entry.target);
                if (temp_length == this._updates0List.length)
                    i++;
            }
        }

        for(i=0; i<this._updatesPosList.length; ){
            temp_length = this._updatesPosList.length;
            entry = this._updatesPosList[i];
            if(entry && entry.priority >= minPriority)
                this.unscheduleUpdate(entry.target);
            if (temp_length == this._updatesPosList.length)
                i++;
        }
    },

    /** 
     * !#en Checks whether a callback for a given target is scheduled.
     * !#zh ????????????????????????
     * @method isScheduled
     * @param {Function} callback The callback to check.
     * @param {Object} target The target of the callback.
     * @return {Boolean} True if the specified callback is invoked, false if not.
     */
    isScheduled: function(callback, target){
        //key, target
        //selector, target
        cc.assertID(callback, 1508);
        cc.assertID(target, 1509);
        var targetId = getTargetId(target);
        cc.assertID(targetId, 1510);
        
        var element = this._hashForTimers[targetId];

        if (!element) {
            return false;
        }

        if (element.timers == null){
            return false;
        }
        else {
            var timers = element.timers;
            for (var i = 0; i < timers.length; ++i) {
                var timer =  timers[i];

                if (callback === timer._callback){
                    return true;
                }
            }
            return false;
        }
    },

    /**
     * !#en
     * Pause all selectors from all targets.<br/>
     * You should NEVER call this method, unless you know what you are doing.
     * !#zh
     * ?????????????<br/>
     * ????????,????????????
     * @method pauseAllTargets
     */
    pauseAllTargets: function () {
        return this.pauseAllTargetsWithMinPriority(cc.Scheduler.PRIORITY_SYSTEM);
    },

    /**
     * !#en
     * Pause all selectors from all targets with a minimum priority. <br/>
     * You should only call this with kCCPriorityNonSystemMin or higher.
     * !#zh
     * ?????????????????????<br/>
     * ????????????? PRIORITY_NON_SYSTEM_MIN ?????
     * @method pauseAllTargetsWithMinPriority
     * @param {Number} minPriority
     */
    pauseAllTargetsWithMinPriority: function (minPriority) {
        var idsWithSelectors = [];

        var self = this, element, locArrayForTimers = self._arrayForTimers;
        var i, li;
        // Custom Selectors
        for(i = 0, li = locArrayForTimers.length; i < li; i++){
            element = locArrayForTimers[i];
            if (element) {
                element.paused = true;
                idsWithSelectors.push(element.target);
            }
        }

        var entry;
        if(minPriority < 0){
            for(i=0; i<this._updatesNegList.length; i++){
                entry = this._updatesNegList[i];
                if (entry) {
                    if(entry.priority >= minPriority){
                        entry.paused = true;
                        idsWithSelectors.push(entry.target);
                    }
                }
            }
        }

        if(minPriority <= 0){
            for(i=0; i<this._updates0List.length; i++){
                entry = this._updates0List[i];
                if (entry) {
                    entry.paused = true;
                    idsWithSelectors.push(entry.target);
                }
            }
        }

        for(i=0; i<this._updatesPosList.length; i++){
            entry = this._updatesPosList[i];
            if (entry) {
                if(entry.priority >= minPriority){
                    entry.paused = true;
                    idsWithSelectors.push(entry.target);
                }
            }
        }

        return idsWithSelectors;
    },

    /**
     * !#en
     * Resume selectors on a set of targets.<br/>
     * This can be useful for undoing a call to pauseAllCallbacks.
     * !#zh
     * ????????????????<br/>
     * ????? pauseAllCallbacks ?????
     * @method resumeTargets
     * @param {Array} targetsToResume
     */
    resumeTargets: function (targetsToResume) {
        if (!targetsToResume)
            return;

        for (var i = 0; i < targetsToResume.length; i++) {
            this.resumeTarget(targetsToResume[i]);
        }
    },

    /**
     * !#en
     * Pauses the target.<br/>
     * All scheduled selectors/update for a given target won't be 'ticked' until the target is resumed.<br/>
     * If the target is not present, nothing happens.
     * !#zh
     * ???????????<br/>
     * ????????????????<br/>
     * ????????????,????????
     * @method pauseTarget
     * @param {Object} target
     */
    pauseTarget: function (target) {
        cc.assertID(target, 1503);
        var targetId = getTargetId(target);
        cc.assertID(targetId, 1510);

        //customer selectors
        var self = this, 
            element = self._hashForTimers[targetId];
        if (element) {
            element.paused = true;
        }

        //update callback
        var elementUpdate = self._hashForUpdates[targetId];
        if (elementUpdate) {
            elementUpdate.entry.paused = true;
        }
    },

    /**
     * !#en
     * Resumes the target.<br/>
     * The 'target' will be unpaused, so all schedule selectors/update will be 'ticked' again.<br/>
     * If the target is not present, nothing happens.
     * !#zh
     * ?????????????<br/>
     * ????????????????<br/>
     * ????????????,????????
     * @method resumeTarget
     * @param {Object} target
     */
    resumeTarget: function (target) {
        cc.assertID(target, 1504);
        var targetId = getTargetId(target);
        cc.assertID(targetId, 1510);

        // custom selectors
        var self = this,
            element = self._hashForTimers[targetId];

        if (element) {
            element.paused = false;
        }

        //update callback
        var elementUpdate = self._hashForUpdates[targetId];

        if (elementUpdate) {
            elementUpdate.entry.paused = false;
        }
    },

    /**
     * !#en Returns whether or not the target is paused.
     * !#zh ????????????????
     * @method isTargetPaused
     * @param {Object} target
     * @return {Boolean}
     */
    isTargetPaused: function (target) {
        cc.assertID(target, 1505);
        var targetId = getTargetId(target);
        cc.assertID(targetId, 1510);

        // Custom selectors
        var element = this._hashForTimers[targetId];
        if (element) {
            return element.paused;
        }
        var elementUpdate = this._hashForUpdates[targetId];
        if (elementUpdate) {
            return elementUpdate.entry.paused;
        }
        return false;
    },

    /**
     * !#en
     * Schedules the 'update' callback_fn for a given target with a given priority.<br/>
     * The 'update' callback_fn will be called every frame.<br/>
     * The lower the priority, the earlier it is called.
     * !#zh
     * ??????? update ????<br/>
     * update ????????????<br/>
     * ???????,??????
     * @method scheduleUpdateForTarget
     * @deprecated since v3.4 please use .scheduleUpdate
     * @param {Object} target
     * @param {Number} priority
     * @param {Boolean} paused
     * @example {@link cocos2d/core/CCScheduler/scheduleUpdateForTarget.js}
     */
    scheduleUpdateForTarget: function(target, priority, paused){
        //cc.log("scheduleUpdateForTarget is deprecated. Please use scheduleUpdate.");
        this.scheduleUpdate(target, priority, paused);
    },

    /**
     * !#en
     * Unschedule a callback function for a given target.<br/>
     * If you want to unschedule the "update", use unscheduleUpdateForTarget.
     * !#zh
     * ?????????????????????????<br/>
     * ?????? update ???,??? unscheduleUpdateForTarget()?
     * @method unscheduleCallbackForTarget
     * @deprecated since v3.4 please use .unschedule
     * @param {Object} target
     * @param {Function} callback - callback[Function] or key[String]
     * @example {@link cocos2d/core/CCScheduler/unscheduleCallbackForTarget.js}
     */
    unscheduleCallbackForTarget: function (target, callback) {
        //cc.log("unscheduleCallbackForTarget is deprecated. Please use unschedule.");
        this.unschedule(callback, target);
    },

    /**
     * !#en Unschedules the update callback function for a given target.
     * !#zh ?????????????
     * @method unscheduleUpdateForTarget
     * @param {Object} target
     * @deprecated since v3.4 please use .unschedule
     * @example {@link cocos2d/core/CCScheduler/unscheduleUpdateForTarget.js}
     */
    unscheduleUpdateForTarget: function (target) {
        //cc.log("unscheduleUpdateForTarget is deprecated. Please use unschedule.");
        this.unscheduleUpdate(target);
    },

    /**
     * !#en
     * Unschedules all function callbacks for a given target.<br/>
     * This also includes the "update" callback function.
     * !#zh ????????????,?? update ????
     * @method unscheduleAllCallbacksForTarget
     * @deprecated since v3.4 please use unscheduleAllForTarget
     * @param {Object} target
     */
    unscheduleAllCallbacksForTarget: function(target){
        //cc.log("unscheduleAllCallbacksForTarget is deprecated. Please use unscheduleAll.");
        this.unscheduleAllForTarget(target);
    },

    /**
     * !#en
     * Unschedules all function callbacks from all targets. <br/>
     * You should NEVER call this method, unless you know what you are doing.
     * !#zh
     * ?????????????<br/>
     * ????????,????????????
     * @method unscheduleAllCallbacks
     * @deprecated since v3.4 please use .unscheduleAllWithMinPriority
     */
    unscheduleAllCallbacks: function(){
        //cc.log("unscheduleAllCallbacks is deprecated. Please use unscheduleAll.");
        this.unscheduleAllWithMinPriority(cc.Scheduler.PRIORITY_SYSTEM);
    },

    /**
     * !#en
     * Unschedules all function callbacks from all targets with a minimum priority.<br/>
     * You should only call this with kCCPriorityNonSystemMin or higher.
     * !#zh
     * ????????????????????????????<br/>
     * ????????????? PRIORITY_NON_SYSTEM_MIN ?????
     * @method unscheduleAllCallbacksWithMinPriority
     * @deprecated since v3.4 please use .unscheduleAllWithMinPriority
     * @param {Number} minPriority
     */
    unscheduleAllCallbacksWithMinPriority: function (minPriority) {
        //cc.log("unscheduleAllCallbacksWithMinPriority is deprecated. Please use unscheduleAllWithMinPriority.");
        this.unscheduleAllWithMinPriority(minPriority);
    }
});

/**
 * !#en Priority level reserved for system services.
 * !#zh ?????????
 * @property PRIORITY_SYSTEM
 * @type {Number}
 * @static
 */
cc.Scheduler.PRIORITY_SYSTEM = 1 << 31;

/**
 * !#en Minimum priority level for user scheduling.
 * !#zh ??????????
 * @property PRIORITY_NON_SYSTEM
 * @type {Number}
 * @static
 */
cc.Scheduler.PRIORITY_NON_SYSTEM = cc.Scheduler.PRIORITY_SYSTEM + 1;
