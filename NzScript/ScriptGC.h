#include <mutex>
#include <unordered_set>
#include <unordered_map>
/// <summary>
/// GC 对象
/// </summary>
class GCObject {
public:
	std::unordered_set<GCObject*> References;
	virtual ~GCObject() {
	}
	virtual const std::type_info& GetType() const noexcept {
		return typeid(GCObject);
	}
	GCObject(class GC& gc);
	void AddRef(GCObject* ref) {
		References.insert(ref);
	}
	void RemoveRef(GCObject* ref) {
		References.erase(ref);
	}
};
/// <summary>
/// GC 上下文类
/// </summary>
class GC {
	std::mutex _lock;
	std::unordered_set<GCObject*> Objects;
	std::unordered_set<GCObject*> Roots;

public:
	void AddRoot(GCObject* obj) {
		std::lock_guard<std::mutex> lock(_lock);
		Roots.insert(obj);
		Objects.insert(obj);
	}

	void RemoveRoot(GCObject* obj) {
		std::lock_guard<std::mutex> lock(_lock);
		Roots.erase(obj);
	}

	void AddObject(GCObject* obj) {
		std::lock_guard<std::mutex> lock(_lock);
		Objects.insert(obj);
	}

	void Collect() {
		std::lock_guard<std::mutex> lock(_lock);

		std::unordered_set<GCObject*> marked;
		std::unordered_set<GCObject*> unmarked;

		// Mark all objects reachable from the roots
		for (auto root : Roots) {
			Mark(root, marked);
		}

		// Unmark all objects that are not reachable from the roots
		for (auto obj : Objects) {
			if (marked.find(obj) == marked.end()) {
				unmarked.insert(obj);
			}
		}

		// Delete all unmarked objects
		for (auto obj : unmarked) {
			Objects.erase(obj);
			delete obj;
		}
	}

private:
	void Mark(GCObject* obj, std::unordered_set<GCObject*>& marked) {
		if (marked.find(obj) != marked.end()) {
			return;
		}

		marked.insert(obj);

		for (auto ref : obj->References) {
			Mark(ref, marked);
		}
	}
};
GCObject::GCObject(GC& gc) {
	gc.AddObject(this);
}