#include <mutex>
#include <unordered_set>
#include <unordered_map>
#include <iostream>
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
class GCString : public GCObject {
public:
	char* Pointer;
	GCString(GC& gc, const char* s) : Pointer(_strdup(s)), GCObject(gc) {}
	virtual const std::type_info& GetType() const noexcept {
		return typeid(GCString);
	}
	~GCString() {
		free(Pointer);
	}
};
/// <summary>
/// GC 上下文类
/// </summary>
class GC {
	std::mutex _lock;
	std::unordered_set<GCObject*> Objects;
	std::unordered_map<GCObject*, int> Roots;

public:
	size_t ObjectCount() {
		return Objects.size();
	}
	void AddRoot(GCObject* obj) {
		std::lock_guard<std::mutex> lock(_lock);
		Roots[obj]++;
		Objects.insert(obj);
	}

	void RemoveRoot(GCObject* obj) {
		std::lock_guard<std::mutex> lock(_lock);
		Roots[obj]--;
		if (Roots[obj] == 0)
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
			Mark(root.first, marked);
		}

		// Unmark all objects that are not reachable from the roots
		for (auto obj : Objects) {
			if (marked.find(obj) == marked.end()) {
				unmarked.insert(obj);
			}
		}

		// Delete all unmarked objects
		if (unmarked.size() == 0)
			return;
		std::cerr << "Erased " << unmarked.size() << " objects.\n";
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