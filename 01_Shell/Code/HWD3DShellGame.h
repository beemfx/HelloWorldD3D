//

#include "HWD3DGame.h"

class HWD3DGame_Shell : public HWD3DGame
{
public:
	
	virtual void Init(HWND InMainWnd) override;
	virtual void Deinit() override;

	virtual void Update(float DeltaTime) override;
	virtual void Render() override;
};
