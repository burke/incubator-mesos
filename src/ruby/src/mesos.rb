# Licensed to the Apache Software Foundation (ASF) under one
# or more contributor license agreements.  See the NOTICE file
# distributed with this work for additional information
# regarding copyright ownership.  The ASF licenses this file
# to you under the Apache License, Version 2.0 (the
# "License"); you may not use this file except in compliance
# with the License.  You may obtain a copy of the License at
#
#   http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

# See include/mesos/scheduler.hpp, include/mesos/executor.hpp and
# include/mesos/mesos.proto for more information documenting this
# interface.

# Ruby bindings for Mesos.

# Alias the implementations from _mesos.

# TODO(wickman) Make Mesos{Scheduler,Executor}DriverImpl inherit from the
# superclasses defined here.

# MesosSchedulerDriver = _mesos.MesosSchedulerDriverImpl
# MesosExecutorDriver = _mesos.MesosExecutorDriverImpl


# Base class for Mesos schedulers. Users' schedulers should extend this
# class to get default implementations of methods they don't override.
class Scheduler

  # Invoked when the scheduler successfully registers with a Mesos master.
  # It is called with the framework_id, a unique ID generated by the
  # master, and the master_info which is information about the master
  # itself.
  def registered(driver, framework_id, master_info)
  end

  # Invoked when the scheduler re-registers with a newly elected Mesos
  # master.  This is only called when the scheduler has previously been
  # registered.  master_info contains information about the newly elected
  # master.
  def reregistered(driver, master_info)
  end

  # Invoked when the scheduler becomes disconnected from the master, e.g.
  # the master fails and another is taking over.
  def disconnected(driver)
  end

  # Invoked when resources have been offered to this framework. A single
  # offer will only contain resources from a single slave.  _resources
  # associated with an offer will not be re-offered to _this_ framework
  # until either (a) this framework has rejected those resources (see
  # SchedulerDriver.launch_tasks) or (b) those resources have been
  # rescinded (see Scheduler.offer_rescinded).  Note that resources may be
  # concurrently offered to more than one framework at a time (depending
  # on the allocator being used).  In that case, the first framework to
  # launch tasks using those resources will be able to use them while the
  # other frameworks will have those resources rescinded (or if a
  # framework has already launched tasks with those resources then those
  # tasks will fail with a TASK_LOST status and a message saying as much).
  def resource_offers(driver, offers)
  end

  # Invoked when an offer is no longer valid (e.g., the slave was lost or
  # another framework used resources in the offer.) If for whatever reason
  # an offer is never rescinded (e.g., dropped message, failing over
  # framework, etc.), a framwork that attempts to launch tasks using an
  # invalid offer will receive TASK_LOST status updats for those tasks
  # (see Scheduler.resource_offers).
  def offer_rescinded(driver, offer_id)
  end

  # Invoked when the status of a task has changed (e.g., a slave is lost
  # and so the task is lost, a task finishes and an executor sends a
  # status update saying so, etc.) Note that returning from this callback
  # acknowledges receipt of this status update.  If for whatever reason
  # the scheduler aborts during this callback (or the process exits)
  # another status update will be delivered.  Note, however, that this is
  # currently not true if the slave sending the status update is lost or
  # fails during that time.
  def status_update(driver, status)
  end

  # Invoked when an executor sends a message. These messages are best
  # effort; do not expect a framework message to be retransmitted in any
  # reliable fashion.
  def framework_message(driver, executor_id, slave_id, message)
  end

  # Invoked when a slave has been determined unreachable (e.g., machine
  # failure, network partition.) Most frameworks will need to reschedule
  # any tasks launched on this slave on a new slave.
  def slave_lost(driver, slave_id)
  end

  # Invoked when an executor has exited/terminated. Note that any tasks
  # running will have TASK_LOST status updates automatically generated.
  def executor_lost(driver, executor_id, slave_id, status)
  end

  # Invoked when there is an unrecoverable error in the scheduler or
  # scheduler driver.  The driver will be aborted BEFORE invoking this
  # callback.
  def error(driver, message)
    puts "Error from Mesos: #{message}"
  end


end

class SchedulerDriver
  # Interface for Mesos scheduler drivers. Users may wish to implement this
  # class in mock objects for tests.

  # Starts the scheduler driver.  This needs to be called before any other
  # driver calls are made.
  def start
  end

  # Stops the scheduler driver. If the 'failover' flag is set to False
  # then it is expected that this framework will never reconnect to Mesos
  # and all of its executors and tasks can be terminated.  Otherwise, all
  # executors and tasks will remain running (for some framework specific
  # failover timeout) allowing the scheduler to reconnect (possibly in the
  # same process, or from a different process, for example, on a different
  # machine.)
  def stop(failover=False)
  end

  # Aborts the driver so that no more callbacks can be made to the
  # scheduler.  The semantics of abort and stop have deliberately been
  # separated so that code can detect an aborted driver (i.e., via the
  # return status of SchedulerDriver.join), and instantiate and start
  # another driver if desired (from within the same process.)
  def abort
  end

  # Waits for the driver to be stopped or aborted, possibly blocking the
  # current thread indefinitely.  The return status of this function can
  # be used to determine if the driver was aborted (see mesos.proto for a
  # description of Status).
  def join
  end

  # Starts and immediately joins (i.e., blocks on) the driver.
  def run
  end

  # Requests resources from Mesos (see mesos.proto for a description of
  # Request and how, for example, to request resources from specific
  # slaves.)  Any resources available are offered to the framework via
  # Scheduler.resource_offers callback, asynchronously.
  def request_resources(requests)
  end

  # Launches the given set of tasks. Any resources remaining (i.e., not
  # used by the tasks or their executors) will be considered declined.
  # The specified filters are applied on all unused resources (see
  # mesos.proto for a description of Filters.) Invoking this function with
  # an empty collection of tasks declines this offer in its entirety (see
  # Scheduler.decline_offer).  Note that currently tasks can only be
  # launched per offer.  In the future, frameworks will be allowed to
  # aggregate offers (resources) to launch their tasks.
  def launch_tasks(offer_id, tasks, filters=None)
  end

  # Kills the specified task. Note that attempting to kill a task is
  # currently not reliable.  If, for example, a scheduler fails over while
  # it was attempting to kill a task it will need to retry in the future
  # (these semantics may be changed in the future).
  def kill_task(task_id)
  end

  # Declines an offer in its entirety and applies the specified
  # filters on the resources (see mesos.proto for a description of
  # Filters). Note that this can be done at any time, it is not
  # necessary to do this within the Scheduler::resource_offers
  # callback.
  def decline_offer(offer_id, filters=None)
  end

  # Removes all filters previously set by the framework (via
  # launch_tasks()).  This enables the framework to receive offers from
  # those filtered slaves.
  def revive_offers
  end

  # Sends a message from the framework to one of its executors. These
  # messages are best effort; do not expect a framework message to be
  # retransmitted in any reliable fashion.
  def send_framework_message(executor_id, slave_id, data)
  end

end


# Base class for Mesos executors. Users' executors should extend this
# class to get default implementations of methods they don't override.
class Executor

  # Invoked once the executor driver has been able to successfully connect
  # with Mesos.  In particular, a scheduler can pass some data to its
  # executors through the Framework_info.Executor_info's data field.
  def registered(driver, executor_info, framework_info, slave_info)
  end

  # Invoked when the executor re-registers with a restarted slave.
  def reregistered(driver, slave_info)
  end

  # Invoked when the executor becomes "disconnected" from the slave (e.g.,
  # the slave is being restarted due to an upgrade).
  def disconnected(driver)
  end

  # Invoked when a task has been launched on this executor (initiated via
  # Scheduler.launch_tasks).  Note that this task can be realized with a
  # thread, a process, or some simple computation, however, no other
  # callbacks will be invoked on this executor until this callback has
  # returned.
  def launch_task(driver, task)
  end

  # Invoked when a task running within this executor has been killed (via
  # SchedulerDriver.kill_task).  Note that no status update will be sent on
  # behalf of the executor, the executor is responsible for creating a new
  # _taskStatus (i.e., with TASK_KILLED) and invoking ExecutorDriver's
  # send_status_update.
  def kill_task(driver, task_id)
  end

  # Invoked when a framework message has arrived for this executor.  These
  # messages are best effort; do not expect a framework message to be
  # retransmitted in any reliable fashion.
  def framework_message(driver, message)
  end

  # Invoked when the executor should terminate all of its currently
  # running tasks.  Note that after Mesos has determined that an executor
  # has terminated any tasks that the executor did not send terminal
  # status updates for (e.g., TASK_KILLED, TASK_FINISHED, TASK_FAILED,
  # etc) a TASK_LOST status update will be created.
  def shutdown(driver)
  end

  # Invoked when a fatal error has occured with the executor and/or
  # executor driver.  The driver will be aborted BEFORE invoking this
  # callback.
  def error(driver, message)
    puts "Error from Mesos: #{message}"
  end


end


# Interface for Mesos executor drivers. Users may wish to extend this
# class in mock objects for tests.
class ExecutorDriver

  # Starts the executor driver. This needs to be called before any other
  # driver calls are made.
  def start
  end

  # Stops the executor driver.
  def stop
  end

  # Aborts the driver so that no more callbacks can be made to the
  # executor.  The semantics of abort and stop have deliberately been
  # separated so that code can detect an aborted driver (i.e., via the
  # return status of ExecutorDriver.join), and instantiate and start
  # another driver if desired (from within the same process, although this
  # functionality is currently not supported for executors).
  def abort
  end

  # Waits for the driver to be stopped or aborted, possibly blocking the
  # current thread indefinitely.  The return status of this function can
  # be used to determine if the driver was aborted (see mesos.proto for a
  # description of Status).
  def join
  end

  # Starts and immediately joins (i.e., blocks on) the driver.
  def run
  end

  # Sends a status update to the framework scheduler, retrying as
  # necessary until an acknowledgement has been received or the executor
  # is terminated (in which case, a TASK_LOST status update will be sent).
  # See Scheduler.status_update for more information about status update
  # acknowledgements.
  def send_status_update(status)
  end

  # Sends a message to the framework scheduler. These messages are best
  # effort; do not expect a framework message to be retransmitted in any
  # reliable fashion.
  def send_framework_message(data)
  end
end
