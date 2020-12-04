#ifndef DONT_OPTIMIZE_AWAY_H
#define DONT_OPTIMIZE_AWAY_H

void fake_reference(char* ptr);

template <typename T>
inline void dont_optimize_away(T&& datum)
{
    fake_reference(reinterpret_cast<char*>(&datum));
}

#endif // DONT_OPTIMIZE_AWAY_H
