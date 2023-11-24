#include "commands.h"

namespace commands {
	int getIdx(const std::vector<QString>& vec, const QString& str) {
		for (int i = 0; i < vec.size(); ++i) {
			if (vec[i] == str) return i;
		}
		return -1;
	}

	int getIdxImpl(const QString& str) {
		return getIdx(impl, str);
	}

	int getIdxComm(const QString& str) {
		return getIdx(comm, str);
	}

	int getIdxInst(const QString& str) {
		return getIdx(inst, str);
	}
}
