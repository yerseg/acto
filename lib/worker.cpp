#include "runtime.h"
#include "worker.h"

namespace acto {
namespace core {

worker_t::worker_t(callbacks* const slots)
  : slots_(slots)
  , thread_(&worker_t::execute, this)
{
  complete_.reset();
}

worker_t::~worker_t() {
  active_ = false;
  event_.signaled();
  complete_.wait();

  if (thread_.joinable()) {
    thread_.join();
  }
}

void worker_t::assign(object_t* const obj, const clock_t slice) {
  assert(!object_ && obj);

  object_ = obj;
  start_ = clock();
  time_ = slice;
  // Так как поток оперирует объектом, то необходимо захватить
  // ссылку на объект, иначе объект может быть удален
  // во время обработки сообщений
  runtime_t::instance()->acquire(obj);
  // Активировать поток
  event_.signaled();
}

void worker_t::wakeup() {
  event_.signaled();
}

void worker_t::execute() {
  while (active_) {
    //
    // Если данному потоку назначен объект, то необходимо
    // обрабатывать сообщения, ему поступившие
    //
    if (!process()) {
      slots_->push_idle(this);
    }

    //
    // Ждать, пока не появится новое задание для данного потока
    //
    event_.wait();  // Cond: (m_object != 0) || (m_active == false)
  }

  complete_.signaled();
}

bool worker_t::check_deleting(object_t* const obj) {
  // TN: В контексте рабочего потока
  std::lock_guard<std::recursive_mutex> g(obj->cs);

  if (obj->has_messages()) {
    return false;
  }
  if (!obj->exclusive || obj->deleting) {
    obj->scheduled = false;
    object_ = nullptr;
  }
  // Если текущий объект необходимо удалить
  if (obj->deleting) {
    return true;
  }

  return false;
}

bool worker_t::process() {
  while (object_t* const obj = object_) {
    bool released = false;

    while (auto msg = obj->select_message()) {
      assert(obj->impl);

      // Обработать сообщение.
      slots_->handle_message(std::move(msg));

      // Проверить истечение лимита времени обработки для данного объекта.
      if (!obj->exclusive && ((clock() - start_) > time_)) {
        std::lock_guard<std::recursive_mutex> g(obj->cs);

        if (!obj->deleting) {
          assert(obj->impl);

          if (obj->has_messages()) {
            runtime_t::instance()->release(obj);

            slots_->push_object(obj);

            released = true;
          } else {
            obj->scheduled = false;
          }
          object_ = nullptr;
          break;
        }
      }
    }

    if (!released && check_deleting(obj)) {
      slots_->push_delete(obj);
    }

    //
    // Получить новый объект для обработки, если он есть в очереди
    //
    if (object_) {
      if (object_->exclusive) {
        return true;
      }
    } else {
      // Освободить ссылку на предыдущий объект
      if (!released) {
        runtime_t::instance()->release(obj);
      }
      /// Retrieve next object from the queue.
      if ((object_ = slots_->pop_object())) {
        start_ = clock();
        runtime_t::instance()->acquire(object_);
      } else {
        // Поместить текущий поток в список свободных
        return false;
      }
    }
  }
  return true;
}

} // namespace core
} // namespace acto
