/*
Copyright (C) 2013 Christopher A. Greeley

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), 
to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, 
and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, 
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, 
WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/
#include <iostream>
#include <string>
#include <vector>
#include <tuple>
#include <typeinfo>
#include <stdio.h>
#include <string.h>
#include <memory>

/*//////////////////////////////////////////////////////
//Author: Christopher A. Greeley ///////////////////////
//Date Created: 2013/11/3 (November 3rd 2013) //////////
//Classification: Library //////////////////////////////
//Name of Work: Call It Later //////////////////////////
//Version: 0.01 ////////////////////////////////////////
//Description: A Small library made to make ////////////
//using std::tuple easier, by doing for clients ////////
//what everyone wants to do with std::tuple.... ////////
//store arguments to functions, and methods ////////////
//and "un - package" them later to plug into a /////////
//function, and, or method./////////////////////////////
//Original Intended Use: Thread It library for /////////
//White - Storm: Eye Of The Storm.//////////////////////
//Developer Contact Information: ///////////////////////
// * : Typical Alias: The Floating Brain ///////////////
// * : Preferred E - Mail: TheFloatingBrain@gmail.com //
// * : Git - Hub: TheFloatingBrain /////////////////////
//////////////////////////////////////////////////////*/


namespace CallItLater
{
	//So no templates have to be deduced.//
	struct AppliedProcedure
	{
		/*NOTE: If this function returns false, it does NOT mean a call failed, 
		it may be that the function is void.*/
		bool GetResultIsValid() {
			return resultIsValid;
		}
		//The execution of the procedure with applied data.//
		virtual void ExecuteFunction() = 0;
		protected: 
			bool resultIsValid;
	};

	//In case the user wants to be more specific.//
	template< typename CLASS_T >
	struct AppliedMethod : public AppliedProcedure
	{
		CLASS_T* GetInstance() {
			return instance;
		}
		void SetInstance( CLASS_T* instance_ ) {
			instance = instance_;
		}
		protected: 
			CLASS_T* instance;
	};

	//I.E Advanced users. "Backend" may deture users, although that is what this should be called.//
	namespace Advanced
	{
		//So different *flavors* of abstraction can be applied.//

		//So only one template needs to be deduced.//
		template< typename RETURN_TYPE_T >
		struct AppliedProcedureWithResult
		{
			RETURN_TYPE_T GetResult() {
				return result;
			}
			protected: 
				RETURN_TYPE_T result;
		};

		template< typename... ARGUMENTS_T >
		struct ProcedureApplication
		{
			std::tuple< ARGUMENTS_T... > GetArgumentData() {
				return argumentData;
			}
			protected: 
				//The arguments to be applied to the procedure.//
				std::tuple< ARGUMENTS_T... > argumentData;
		};


		template< typename RETURN_TYPE_T >
		struct ResultableAppliedProcedure : 
				public AppliedProcedure, public AppliedProcedureWithResult< RETURN_TYPE_T > {
		};

		template< typename... ARGUMENTS_T >
		struct ProcedureWithApplication : 
				public AppliedProcedure, public ProcedureApplication< ARGUMENTS_T... > {
		};

		//In case the client just wants to abstract away the procedure's pointer itself.//
		template< typename RETURN_TYPE_T, typename... ARGUMENTS_T >
		struct CompleteProcedureApplication : public AppliedProcedure, 
				public ProcedureApplication< ARGUMENTS_T... >, public AppliedProcedureWithResult< RETURN_TYPE_T > {
		};

		template< typename CLASS_TYPE_T, typename RETURN_TYPE_T >
		struct ResultableAppliedMethod : 
				public AppliedMethod< CLASS_TYPE_T >, public AppliedProcedureWithResult< RETURN_TYPE_T > {
		};

		template< typename CLASS_TYPE_T, typename... ARGUMENTS_T >
		struct MethodWithApplication : 
				public AppliedMethod< CLASS_TYPE_T >, public ProcedureApplication< ARGUMENTS_T... > {
		};

		//In case the client just wants to abstract away the procedure's pointer itself.//
		template< typename CLASS_TYPE_T, typename RETURN_TYPE_T, typename... ARGUMENTS_T >
		struct CompleteMethodApplication : public AppliedMethod< CLASS_TYPE_T >, 
				public ProcedureApplication< ARGUMENTS_T... >, public AppliedProcedureWithResult< RETURN_TYPE_T > {
		};

		//Functor, so a partial template - specialization can be used which is needed to terminate the recursion at compile - time.//
		//Both a RETURN_TYPE_T/ARGUMENTS_T, and a FUNCTION_POINTER_T, in the case of a void function (see - below).//
		template< int IETORATOR_T, typename RETURN_TYPE_T, typename TUPLE, typename FUNCTION_POINTER_T, typename... ARGUMENTS_T >
		struct FunctionRunner
		{
			//So the return value of the function can be obtained.//
			RETURN_TYPE_T result;
			FunctionRunner( TUPLE tuple, FUNCTION_POINTER_T functionToRun, ARGUMENTS_T... arguments )
			{
				//Recursively iteratorate through the tuple, buffering its elements with each successive instantiation, so that the buffered data may be used later.//
				FunctionRunner< IETORATOR_T - 1, RETURN_TYPE_T, TUPLE, FUNCTION_POINTER_T, decltype( std::get< IETORATOR_T >( tuple ) ), ARGUMENTS_T... > runner{ 
						tuple, functionToRun, std::get< IETORATOR_T >( tuple ), arguments... };
				//After the recursion is over, get the result from the ladder instance.//
				result = runner.result;
			}
		};

		/*When the previous definition of this class passes in -1 for the first template argument in the constructor, then, 
		this specialization will be instantiated instead, and the function will be called via function pointer, and the arguments
		stored in the tuple will be passed in.*/
		template< typename RETURN_TYPE_T, typename TUPLE, typename FUNCTION_POINTER_T, typename... ARGUMENTS_T >
		struct FunctionRunner< ( -1 ), RETURN_TYPE_T, TUPLE, FUNCTION_POINTER_T, ARGUMENTS_T... >
		{
			//So the return value of the function can be obtained.//
			RETURN_TYPE_T result;
			//Actually use the data.//
			FunctionRunner( TUPLE tuple, FUNCTION_POINTER_T functionToRun, ARGUMENTS_T... arguments ) {
				//Call the function, apply the arguments, and get the result.//
				result = functionToRun( arguments... );
			}
		};

		//In the case that the function is void.//
		template< int IETORATOR_T, typename TUPLE, typename FUNCTION_POINTER_T, typename... ARGUMENTS_T >
		struct FunctionRunner< IETORATOR_T, void, TUPLE, FUNCTION_POINTER_T, ARGUMENTS_T... >
		{
			FunctionRunner( TUPLE tuple, FUNCTION_POINTER_T functionToRun, ARGUMENTS_T... arguments )
			{
				//Recursively iteratorate through the tuple, buffering its elements with each successive instantiation, so that the buffered data may be used later.//
				FunctionRunner< IETORATOR_T - 1, void, TUPLE, FUNCTION_POINTER_T, decltype( std::get< IETORATOR_T >( tuple ) ), ARGUMENTS_T... > runner{ 
						tuple, functionToRun, std::get< IETORATOR_T >( tuple ), arguments... };
			}
		};

		//In the case that the function is void.//
		template< typename TUPLE, typename FUNCTION_POINTER_T, typename... ARGUMENTS_T >
		struct FunctionRunner< ( -1 ), void, TUPLE, FUNCTION_POINTER_T, ARGUMENTS_T... >
		{
			//Actually use the data.//
			FunctionRunner( TUPLE tuple, FUNCTION_POINTER_T functionToRun, ARGUMENTS_T... arguments ) {
				//Call the function, and apply the arguments.//
				functionToRun( arguments... );
			}
		};

		//Functor, so a partial template - specialization can be used which is needed to terminate the recursion at compile - time.//
		//Both a RETURN_TYPE_T/ARGUMENTS_T, and a FUNCTION_POINTER_T, in the case of a void function (see - below).//
		template< int IETORATOR_T, typename CLASS_TYPE_T, typename RETURN_TYPE_T, typename TUPLE, typename DELEGATE_T, typename... ARGUMENTS_T >
		struct MethodRunner
		{
			//So the return value of the function can be obtained.//
			RETURN_TYPE_T result;
			MethodRunner( CLASS_TYPE_T* classInstance, TUPLE tuple, DELEGATE_T delegate, ARGUMENTS_T... arguments )
			{
				//Recursively iteratorate through the tuple, buffering its elements with each successive instantiation, so that the buffered data may be used later.//
				MethodRunner< IETORATOR_T - 1, CLASS_TYPE_T, RETURN_TYPE_T, TUPLE, DELEGATE_T, decltype( std::get< IETORATOR_T >( tuple ) ), ARGUMENTS_T... > runner{ 
						classInstance, tuple, delegate, std::get< IETORATOR_T >( tuple ), arguments... };
				//After the recursion is over, get the result from the ladder instance.//
				result = runner.result;
			}
		};

		/*When the previous definition of this class passes in -1 for the first template argument in the constructor, then, 
		this specialization will be instantiated instead, and the function will be called via function pointer, and the arguments
		stored in the tuple will be passed in.*/
		template< typename CLASS_TYPE_T, typename RETURN_TYPE_T, typename TUPLE, typename DELEGATE_T, typename... ARGUMENTS_T >
		struct MethodRunner< ( -1 ), CLASS_TYPE_T, RETURN_TYPE_T, TUPLE, DELEGATE_T, ARGUMENTS_T... >
		{
			//So the return value of the function can be obtained.//
			RETURN_TYPE_T result;
			//Actually use the data.//
			MethodRunner( CLASS_TYPE_T* classIntance, TUPLE tuple, DELEGATE_T delegate, ARGUMENTS_T... arguments ) {
				//Call the function, apply the arguments, and get the result.//
				result = ( classIntance->*delegate )( arguments... );
			}
		};

		//In the case that the function is void.//
		template< int IETORATOR_T, typename CLASS_TYPE_T, typename TUPLE, typename DELEGATE_T, typename... ARGUMENTS_T >
		struct MethodRunner< IETORATOR_T, CLASS_TYPE_T, void, TUPLE, DELEGATE_T, ARGUMENTS_T... >
		{
			MethodRunner( CLASS_TYPE_T* classIntance, TUPLE tuple, DELEGATE_T delegate, ARGUMENTS_T... arguments )
			{
				//Recursively iteratorate through the tuple, buffering its elements with each successive instantiation, so that the buffered data may be used later.//
				MethodRunner< IETORATOR_T - 1, CLASS_TYPE_T, void, TUPLE, DELEGATE_T, decltype( std::get< IETORATOR_T >( tuple ) ), ARGUMENTS_T... > runner{ 
						classIntance, tuple, delegate, std::get< IETORATOR_T >( tuple ), arguments... };
			}
		};

		//In the case that the function is void.//
		template< typename CLASS_TYPE_T, typename TUPLE, typename DELEGATE_T, typename... ARGUMENTS_T >
		struct MethodRunner< ( -1 ), CLASS_TYPE_T, void, TUPLE, DELEGATE_T, ARGUMENTS_T... >
		{
			//Actually use the data.//
			MethodRunner( CLASS_TYPE_T* classIntance, TUPLE tuple, DELEGATE_T delegate, ARGUMENTS_T... arguments ) {
				//Call the function, and apply the arguments.//
				( classIntance->*delegate )( arguments... );
			}
		};


		/*Description: 
		Allows a client to pass an arbitrary function pointer, with an arbitrary amount of 
		arguments to an instance of this class, and call that function, with the specified 
		arguments, at arbitrary intervals, and obtain the result (return - value) of the function.*/
		template< typename RETURN_TYPE_T, typename... ARGUMENT_TYPES_T >
		struct SpecefiedArgumentApplicableFunctionContainer : public CompleteProcedureApplication< RETURN_TYPE_T, ARGUMENT_TYPES_T... >
		{
			//A pointer to the function to be run, and apply the arguments too.//
			RETURN_TYPE_T (* functionToApplyArgumentsTo )( ARGUMENT_TYPES_T... );
			explicit SpecefiedArgumentApplicableFunctionContainer( RETURN_TYPE_T (* functionToApplyArgumentsTo_ )( ARGUMENT_TYPES_T... ), ARGUMENT_TYPES_T... arguments ) : 
					functionToApplyArgumentsTo( functionToApplyArgumentsTo_ ) {
				this->resultIsValid = false;
				this->argumentData = std::tuple< ARGUMENT_TYPES_T... >( std::forward< ARGUMENT_TYPES_T >( arguments )... );
			}
			virtual void ExecuteFunction()
			{
				/*Instantaite a runner, wich will recursivly instantaite runners, unpacking the arguments, 
				and calling the function via function pointer pointer.*/
				FunctionRunner< ( std::tuple_size< decltype( this->argumentData ) >::value - 1 ), 
						RETURN_TYPE_T, std::tuple< ARGUMENT_TYPES_T... >, RETURN_TYPE_T (*)( ARGUMENT_TYPES_T... ) > runner{ this->argumentData, this->functionToApplyArgumentsTo };
				//Get the result of the execution, and mark it as valid.//
				this->result = runner.result;
				this->resultIsValid = true;
			}
		};

		//In the case that the function is void.//
		template< typename... ARGUMENT_TYPES_T >
		struct SpecefiedArgumentApplicableFunctionContainer< void, ARGUMENT_TYPES_T... > : public CompleteProcedureApplication< void*, ARGUMENT_TYPES_T... >
		{
			//A pointer to the function to be run, and apply the arguments too.//
			void (* functionToApplyArgumentsTo )( ARGUMENT_TYPES_T... );
			explicit SpecefiedArgumentApplicableFunctionContainer( void (* functionToApplyArgumentsTo_ )( ARGUMENT_TYPES_T... ), ARGUMENT_TYPES_T... arguments ) : 
					functionToApplyArgumentsTo( functionToApplyArgumentsTo_ )
			{
				this->resultIsValid = false;
				this->result = NULL;
				this->argumentData = std::tuple< ARGUMENT_TYPES_T... >( std::forward< ARGUMENT_TYPES_T >( arguments )... );
			}
			virtual void ExecuteFunction()
			{
				/*Instantaite a runner, wich will recursivly instantaite runners, unpacking the arguments, 
				and calling the function via function pointer pointer.*/
				FunctionRunner< ( std::tuple_size< decltype( this->argumentData ) >::value - 1 ), 
						void, std::tuple< ARGUMENT_TYPES_T... >, void (*)( ARGUMENT_TYPES_T... ) > runner{ this->argumentData, this->functionToApplyArgumentsTo };
			}
		};

		/*Description: 
		Allows a client to pass an arbitrary delegate, with an arbitrary amount of 
		arguments to an instance of this class, and call that method, with the specified 
		arguments, at arbitrary intervals, and obtain the result (return - value) of the method.*/
		template< typename CLASS_TYPE_T, typename RETURN_TYPE_T, typename... ARGUMENT_TYPES_T >
		struct SpecefiedArgumentApplicableMethodContainer : public CompleteMethodApplication< CLASS_TYPE_T, RETURN_TYPE_T, ARGUMENT_TYPES_T... >
		{
			//A pointer to the function to be run, and apply the arguments too.//
			RETURN_TYPE_T (CLASS_TYPE_T::* delegate )( ARGUMENT_TYPES_T... );
			explicit SpecefiedArgumentApplicableMethodContainer( CLASS_TYPE_T* instance_, RETURN_TYPE_T ( CLASS_TYPE_T::* delegate_ )( ARGUMENT_TYPES_T... ), 
					ARGUMENT_TYPES_T... arguments ) : delegate( delegate_ )
			{
				this->instance = instance_;
				this->resultIsValid = false;
				this->argumentData = std::tuple< ARGUMENT_TYPES_T... >( std::forward< ARGUMENT_TYPES_T >( arguments )... );
			}
			virtual void ExecuteFunction()
			{
				/*Instantaite a runner, wich will recursivly instantaite runners, unpacking the arguments, 
				and calling the function via function pointer pointer.*/
				MethodRunner< ( std::tuple_size< decltype( this->argumentData ) >::value - 1 ), 
						CLASS_TYPE_T, RETURN_TYPE_T, std::tuple< ARGUMENT_TYPES_T... >, RETURN_TYPE_T ( CLASS_TYPE_T::* )( ARGUMENT_TYPES_T... ) > runner{ this->instance, this->argumentData, this->delegate };
				//Get the result of the execution, and mark it as valid.//
				this->result = runner.result;
				this->resultIsValid = true;
			}
		};

		//In the case that the function is void.//
		template< typename CLASS_TYPE_T, typename... ARGUMENT_TYPES_T >
		struct SpecefiedArgumentApplicableMethodContainer< CLASS_TYPE_T, void, ARGUMENT_TYPES_T... > : public CompleteMethodApplication< CLASS_TYPE_T, void*, ARGUMENT_TYPES_T... >
		{
			//A pointer to the function to be run, and apply the arguments too.//
			void (CLASS_TYPE_T::* delegate )( ARGUMENT_TYPES_T... );
			explicit SpecefiedArgumentApplicableMethodContainer( CLASS_TYPE_T* instance_, void ( CLASS_TYPE_T::* delegate_ )( ARGUMENT_TYPES_T... ), 
					ARGUMENT_TYPES_T... arguments ) : delegate( delegate_ )
			{
				this->instance = instance_;
				this->resultIsValid = false;
				this->result = NULL;
				this->argumentData = std::tuple< ARGUMENT_TYPES_T... >( std::forward< ARGUMENT_TYPES_T >( arguments )... );
			}
			virtual void ExecuteFunction()
			{
				/*Instantaite a runner, wich will recursivly instantaite runners, unpacking the arguments, 
				and calling the function via function pointer pointer.*/
				MethodRunner< ( std::tuple_size< decltype( this->argumentData ) >::value - 1 ), 
						CLASS_TYPE_T, void, std::tuple< ARGUMENT_TYPES_T... >, void (CLASS_TYPE_T::*)( ARGUMENT_TYPES_T... ) > runner{ this->instance, this->argumentData, this->delegate };
			}
		};
	}
	//Simplify things for the user.//
	template< typename RETURN_TYPE_T, typename... ARGUMENT_TYPES_T >
	std::shared_ptr< AppliedProcedure > MakeAppliedProcedure( 
			RETURN_TYPE_T (* functionToCall )( ARGUMENT_TYPES_T... ), ARGUMENT_TYPES_T... arguments ) {
		return std::make_shared< Advanced::SpecefiedArgumentApplicableFunctionContainer< RETURN_TYPE_T, ARGUMENT_TYPES_T... > >( functionToCall, arguments... );
	}

	//Simplify things for the user.//
	template< typename CLASS_TYPE_T, typename RETURN_TYPE_T, typename... ARGUMENT_TYPES_T >
	std::shared_ptr< AppliedProcedure > MakeAppliedMethod( 
			CLASS_TYPE_T* instance, RETURN_TYPE_T (CLASS_TYPE_T::* delegate )( ARGUMENT_TYPES_T... ), ARGUMENT_TYPES_T... arguments ) {
		return std::make_shared< Advanced::SpecefiedArgumentApplicableMethodContainer< CLASS_TYPE_T, RETURN_TYPE_T, ARGUMENT_TYPES_T... > >( instance, delegate, arguments... );
	}

	//Simplify things for the user.//
	//The return type is what it is so the user can choose what type of pointer they want to use without dangourus casts.//
	template< typename RETURN_TYPE_T, typename... ARGUMENT_TYPES_T >
	std::shared_ptr< Advanced::SpecefiedArgumentApplicableFunctionContainer< RETURN_TYPE_T, ARGUMENT_TYPES_T... > > RawMakeAppliedProcedure( 
			RETURN_TYPE_T (* functionToCall )( ARGUMENT_TYPES_T... ), ARGUMENT_TYPES_T... arguments ) {
		return std::make_shared< Advanced::SpecefiedArgumentApplicableFunctionContainer< RETURN_TYPE_T, ARGUMENT_TYPES_T... > >( functionToCall, arguments... );
	}

	//Simplify things for the user.//
	//The return type is what it is so the user can choose what type of pointer they want to use without dangourus casts.//
	template< typename CLASS_TYPE_T, typename RETURN_TYPE_T, typename... ARGUMENT_TYPES_T >
	std::shared_ptr< Advanced::SpecefiedArgumentApplicableMethodContainer< CLASS_TYPE_T, RETURN_TYPE_T, ARGUMENT_TYPES_T... > > RawMakeAppliedMethod( 
			CLASS_TYPE_T* instance, RETURN_TYPE_T (CLASS_TYPE_T::* delegate )( ARGUMENT_TYPES_T... ), ARGUMENT_TYPES_T... arguments ) {
		return std::make_shared< Advanced::SpecefiedArgumentApplicableMethodContainer< CLASS_TYPE_T, RETURN_TYPE_T, ARGUMENT_TYPES_T... > >( instance, delegate, arguments... );
	}
}
