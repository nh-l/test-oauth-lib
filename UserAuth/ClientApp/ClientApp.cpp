#include "SignIn.h"

#include <Windows.h>

#include <iostream>

int main()
{
    std::wcout.imbue(std::locale(""));

    SignIn signIn;

    std::cout << "Google 로그인 시작" << std::endl;
    {
        auto access_token = signIn.requestSignIn(SIGNIN_WEB::GOOGLE);
        auto profile = signIn.getUserProfile(SIGNIN_WEB::GOOGLE, access_token);

        std::wcout << profile.username << std::endl;
        std::wcout << profile.email << std::endl;
        std::wcout << profile.profileImage << std::endl;

        signIn.openProfileView(SIGNIN_WEB::GOOGLE, access_token);

        if (signIn.checkSignIn(SIGNIN_WEB::GOOGLE, access_token))
        {
            std::cout << "Google 로그인 상태" << std::endl;

            if (signIn.requestSignOut(SIGNIN_WEB::GOOGLE, access_token))
                std::cout << "Google 로그아웃 성공" << std::endl;

            if (signIn.checkSignIn(SIGNIN_WEB::GOOGLE, access_token) == false)
                std::cout << "현재 Google 로그인 상태가 아님" << std::endl;

            signIn.openProfileView(SIGNIN_WEB::GOOGLE, access_token);

        }
    }
    std::cout << "Google 로그인 종료" << std::endl;

    std::cout << std::endl << " ============================================== " << std::endl << std::endl;

    std::cout << "Facebook 로그인 시작" << std::endl;
    {
        auto access_token = signIn.requestSignIn(SIGNIN_WEB::FACEBOOK);
        auto profile = signIn.getUserProfile(SIGNIN_WEB::FACEBOOK, access_token);

        std::wcout << profile.username << std::endl;
        std::wcout << profile.email << std::endl;
        std::wcout << profile.profileImage << std::endl;

        signIn.openProfileView(SIGNIN_WEB::FACEBOOK, access_token);

        if (signIn.checkSignIn(SIGNIN_WEB::FACEBOOK, access_token))
        {
            std::cout << "Facebook 로그인 상태" << std::endl;

            if (signIn.requestSignOut(SIGNIN_WEB::FACEBOOK, access_token))
                std::cout << "Facebook 로그아웃 성공" << std::endl;

            if (signIn.checkSignIn(SIGNIN_WEB::FACEBOOK, access_token) == false)
                std::cout << "현재 Facebook 로그인 상태가 아님" << std::endl;
        }
    }
    std::cout << "Facebook 로그인 종료" << std::endl;
}
