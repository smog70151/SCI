/*
Authors: Mayank Rathee, Deevashwer Rathee
Copyright:
Copyright (c) 2020 Microsoft Research
Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:
The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.
THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#ifndef OT_PACK_H__
#define OT_PACK_H__
#include "../OT/emp-ot.h"
#include "../utils/emp-tool.h"

#define KKOT_TYPES 8

namespace sci
{
	template <typename T>
	class OTPack
	{
	public:
		SplitKKOT<T> *kkot[KKOT_TYPES];

		// iknp_straight and iknp_reversed: SCI_party
		// acts as sender in straight and receiver in reversed.
		// Needed for MUX calls.
		SplitIKNP<T> *iknp_straight;
		SplitIKNP<T> *iknp_reversed;
		T *io;
		int SCI_party;
		bool do_setup = false;

		OTPack(T *io, int SCI_party, bool do_setup = true)
		{
			this->SCI_party = SCI_party;
			this->do_setup = do_setup;
			this->io = io;

			for (int i = 0; i < KKOT_TYPES; i++)
			{
				kkot[i] = new SplitKKOT<NetIO>(SCI_party, io, 1 << (i + 1));
			}

			iknp_straight = new SplitIKNP<NetIO>(SCI_party, io);
			iknp_reversed = new SplitIKNP<NetIO>(3 - SCI_party, io);

			if (do_setup)
			{
				SetupBaseOTs();
			}
		}

		~OTPack()
		{
			for (int i = 0; i < KKOT_TYPES; i++)
				delete kkot[i];
			delete iknp_straight;
			delete iknp_reversed;
		}

		void SetupBaseOTs()
		{
			switch (SCI_party)
			{
			case 1:
				kkot[0]->setup_send();
				iknp_straight->setup_send();
				iknp_reversed->setup_recv();
				for (int i = 1; i < KKOT_TYPES; i++)
				{
					kkot[i]->setup_send(kkot[0]->k0, kkot[0]->s);
				}
				break;
			case 2:
				kkot[0]->setup_recv();
				iknp_straight->setup_recv();
				iknp_reversed->setup_send();
				for (int i = 1; i < KKOT_TYPES; i++)
				{
					kkot[i]->setup_recv(kkot[0]->k0, kkot[0]->k1);
				}
				break;
			}
		}

		OTPack<T> *operator=(OTPack<T> *copy_from)
		{
			assert(this->do_setup == false && copy_from->do_setup == true);
			OTPack<T> *temp = new OTPack<T>(this->io, copy_from->SCI_party, false);
			SplitKKOT<T> *kkot_base = copy_from->kkot[0];
			SplitIKNP<T> *iknp_s_base = copy_from->iknp_straight;
			SplitIKNP<T> *iknp_r_base = copy_from->iknp_reversed;

			switch (SCI_party)
			{
			case 1:
				for (int i = 0; i < KKOT_TYPES; i++)
				{
					temp->kkot[i]->setup_send(kkot_base->k0, kkot_base->s);
				}
				temp->iknp_straight->setup_send(iknp_s_base->k0, iknp_s_base->s);
				temp->iknp_reversed->setup_recv(iknp_r_base->k0, iknp_r_base->k1);
				break;
			case 2:
				for (int i = 0; i < KKOT_TYPES; i++)
				{
					temp->kkot[i]->setup_recv(kkot_base->k0, kkot_base->k1);
				}
				temp->iknp_straight->setup_recv(iknp_s_base->k0, iknp_s_base->k1);
				temp->iknp_reversed->setup_send(iknp_s_base->k0, iknp_s_base->s);
				break;
			}
			temp->do_setup = true;
			return temp;
		}

		void copy(OTPack<T> *copy_from)
		{
			assert(this->do_setup == false && copy_from->do_setup == true);
			SplitKKOT<T> *kkot_base = copy_from->kkot[0];
			SplitIKNP<T> *iknp_s_base = copy_from->iknp_straight;
			SplitIKNP<T> *iknp_r_base = copy_from->iknp_reversed;

			switch (this->SCI_party)
			{
			case 1:
				for (int i = 0; i < KKOT_TYPES; i++)
				{
					this->kkot[i]->setup_send(kkot_base->k0, kkot_base->s);
				}
				this->iknp_straight->setup_send(iknp_s_base->k0, iknp_s_base->s);
				this->iknp_reversed->setup_recv(iknp_r_base->k0, iknp_r_base->k1);
				break;
			case 2:
				for (int i = 0; i < KKOT_TYPES; i++)
				{
					this->kkot[i]->setup_recv(kkot_base->k0, kkot_base->k1);
				}
				this->iknp_straight->setup_recv(iknp_s_base->k0, iknp_s_base->k1);
				this->iknp_reversed->setup_send(iknp_r_base->k0, iknp_r_base->s);
				break;
			}
			this->do_setup = true;
			return;
		}
	};
} // namespace sci
#endif // OT_PACK_H__
