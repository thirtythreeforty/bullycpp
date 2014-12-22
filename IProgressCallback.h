#ifndef IPROGRESSCALLBACK
#define IPROGRESSCALLBACK

class IProgressCallback {
public:
	enum class Status {
		Idle,        // always 100%
		Busy,        // no % given
		Programming,
		Verifying,
		Error        // no % given
	};
	virtual void onProgress(Status, int) = 0;
};

#endif // IPROGRESSCALLBACK
