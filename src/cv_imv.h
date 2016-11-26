#ifndef CV_IMV_H
#define CV_IMV_H

#include <stdint.h>

/* TODO: Packed structure? */
typedef struct {
	int8_t x;
	int8_t y;
	uint16_t sad;
} cv_imv_t;

class cv_imv
{
	cv_imv_t *m_imv = NULL;
	size_t m_size;
	int m_mbx;
	int m_mby;
	unsigned long m_timestamp;

public:
	cv_imv(uint8_t *p_buffer, int mbx, int mby, unsigned long timestamp)
	{
		m_mbx = mbx;
		m_mby = mby;
		m_size = (m_mbx+1) * (m_mby);
		m_timestamp = timestamp;
		m_imv = new cv_imv_t[m_size];
		memcpy(m_imv, p_buffer, m_size * sizeof(cv_imv_t));
	}

	cv_imv(cv_imv const& copy)
	{
		/* Comes from http://stackoverflow.com/a/255744 */
		m_size = copy.m_size;
		m_imv = new cv_imv_t[m_size];
		std::copy(&copy.m_imv[0], &copy.m_imv[copy.m_size], m_imv);
	}

	cv_imv& operator=(cv_imv rhs)
	{
		rhs.swap(*this);
		return *this;
	}

	void swap(cv_imv& s) noexcept
	{
		std::swap(m_imv, s.m_imv);
		std::swap(m_size, s.m_size);
		std::swap(m_mbx, s.m_mbx);
		std::swap(m_mby, s.m_mby);
		std::swap(m_timestamp, s.m_timestamp);
	}

	~cv_imv()
	{
		delete [] m_imv;
	}

	cv_imv_t *imv()
	{
		return m_imv;
	}

	unsigned long timestamp()
	{
		return m_timestamp;
	}

	int mbx()
	{
		return m_mbx;
	}

	int mby()
	{
		return m_mby;
	}
};

#endif
